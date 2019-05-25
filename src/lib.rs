//! # NDEF Message Handling
//!
//! An NDEF message is a sequence of bytes stored on an NFC tag. The NFC Forum standard defines the protocol in terms of
//! octets, which we implement with the Rust type `u8` and may synonymously refer to as bytes since most platforms have
//! 8 bit bytes.
//!
//! Much of the documentation here is summarized, with segments of copied verbatim, from the NFC Forum NDEF
//! specification.
//!
//! ## Message Contents Structure (in order):
//! - [Message Begin](struct.RecordHeader.html#structfield.mb) - 1 bit (bit 7)
//!     - Required
//!     - Indicates if this is the start of an NDEF message
//! - [Message End](struct.RecordHeader.html#structfield.me) - 1 bit (bit 6)
//!     - Required
//!     - Indicates if this is the last record in the message
//! - [Chunk Flag](struct.RecordHeader.html#structfield.cf) - 1 bit (bit 5)
//!     - Required
//!     - Indicates if this the first record chunk or in the middle
//! - [Short Record Flag](struct.RecordHeader.html#structfield.sr) - 1 bit (bit 4)
//!     - Required
//!     - Indicates if PAYLOAD LENGTH field is short (1 byte, 0-255) or longer
//! - [IL Flag](struct.RecordHeader.html#structfield.il) - 1 bit (bit 3)
//!     - Required
//!     - Indicates whether ID Length Field is present
//! - [Type Name Format](enum.TypeNameFormat.html) - 3 bits (bits 2-0)
//!     - Required
//!     - Can be 0
//! - [Type Length](struct.Record.html#structfield.type_length) - 1 byte
//!     - Will always be 0 for certain values of TNF field
//!     - Specifies length, in octets, of the ID field
//! - [Payload length](struct.Record.html#structfield.payload_length) - 1 byte or 4 bytes
//!     - Required
//!     - Can be 0
//!     - If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 4 byte value
//! - [ID Length](struct.Record.html#structfield.id_length) - 1 byte
//!     - Can be 0 (will result in omission of [ID field](struct.Record.html#structfield.id_field))
//!     - If `IL` flag is set in the record header then this value will be 1 byte in length, otherwise it will be
//!         omitted
//! - [Type](struct.Record.html#structfield.record_type) - length in bytes defined by Type Length field
//!     - Identifier that describes the contents of the payload
//!     - Formed from characters in US ASCII set, characters in range [0-31] and 127 are invalid and SHALL NOT be used
//! - [ID](struct.Record.html#structfield.id_field) - length in bytes defined by ID Length field
//!     - Relative or absolute URI identifier
//!     - Omitted if [ID length](struct.Record.html#structfield.id_length) is 0
//! - [Payload](struct.Record.html#structfield.payload) - length in bytes defined by Payload Length field
//!     - Data in this field is opaque to the library and will be merely passed along
extern crate libc;

use libc::{size_t, uint8_t};
use std::collections::VecDeque;
use std::convert::{From, TryFrom};
use std::iter::FromIterator;
use std::str::from_utf8;
use std::{ptr, slice};

// Library modules
pub mod record_header;
pub mod tnf;
mod types;
pub mod well_known_types;

use record_header::RecordHeader;

/// NDEF record struct
#[derive(Debug, PartialEq)]
pub struct NDEFRecord {
    /// NDEF Record header object
    pub header: RecordHeader,

    /// Specifies length of payload type in octets
    pub type_length: u8,

    /// Specifies length of payload in octets
    ///
    /// If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 32-bit value
    pub payload_length: u32,

    /// Specifies length of ID field in octets
    ///
    /// Only included if the `IL` flag is set in the record header, otherwise omitted entirely
    pub id_length: Option<u8>,

    /// Specifies record type. Must follow the structure, encoding, and format implied by the value of the TNF field. If
    /// a NDEF record is received with a supported TNF field but unknown TYPE field value, the type identifier will be
    /// treated as if the TNF field were `0x05` (Unknown)
    ///
    /// For example, if the TNF is set to 0x01 the record type might be `T` to indicate a text message, `U` for a URI
    /// message. If the TNF is set 0x02 then the record type might be one of "text/json", "text/plain", "image/png",
    /// etc.
    pub record_type: String,

    /// ID field
    ///
    /// Only included if the `IL` flag is set in the record header and the ID_LENGTH field is > 0
    pub id_field: Option<String>,

    /// Payload - A slice of octets, the length of which is declared in
    /// [payload_length](struct.Record.html#structfield.payload_length)
    pub payload: Vec<u8>,
}

// Allows us to convert from the raw bytes collected from C (converted to u8) into a Record struct
impl TryFrom<&[u8]> for NDEFRecord {
    type Error = String;

    fn try_from(value: &[u8]) -> Result<Self, Self::Error> {
        let mut value = VecDeque::from(value.to_vec());

        if value.len() < 4 {
            // There are at least 4 required octets (fields)
            return Err("Invalid number of octets, must have at least 4".to_string());
        }

        // Read first byte into flags and TNF bits - first two bytes are sure to exist due to size check at start
        let header = RecordHeader::new(value.pop_front().unwrap());
        println!("{:#?}", header);
        let type_length: u8 = value.pop_front().unwrap();
        let payload_length: u32 = match header.sr {
            true => value.pop_front().unwrap() as u32,
            false => {
                // Bytes array to build u32 from
                let mut length: [u8; 4] = [0, 0, 0, 0];

                // Attempt to fill bytes array
                for i in 0..3 {
                    length[i] = match value.pop_front() {
                        Some(byte) => byte,
                        None => {
                            return Err(format!(
                                "Too few bytes to create payload length, needed 4 got {}",
                                i
                            ))
                        }
                    };
                }

                // Create a u32 integer from the bytes in the NDEF message, which are in big endian order
                u32::from_be_bytes(length)
            }
        };

        // Default to expecting no ID length
        let mut id_length: Option<u8> = None;
        if header.il {
            id_length = match value.pop_front() {
                Some(len) => Some(len),
                None => return Err("Missing ID length byte".to_string()),
            };
        }

        // Check if there are enough bytes to pull out type field
        if value.len() < type_length as usize {
            return Err(format!(
                "Too few bytes to create ID length field: need {}, have {}",
                type_length,
                value.len()
            ));
        }

        // Checked that the bytes we require are available, now collect them
        let type_bytes: Vec<u8> = value.drain(0..type_length as usize).collect();

        // Create the type field from the bytes, converting them into ASCII characters after validating them
        let mut type_field = String::new();
        for byte in type_bytes.into_iter() {
            if byte < 31 || byte == 127 {
                // Invalid character, no ASCII characters [0-31] or 127
                return Err(
                    format!("Invalid character code {} found in type field", byte).to_string(),
                );
            }

            // Append valid character to type string
            type_field.push(byte as char);
        }

        // Default to expecting no ID field
        let mut id_field: Option<String> = None;
        let mut id_bytes: Vec<u8>;

        // Only attempt to extract ID field if the length is > 0
        let tmp_length = id_length.unwrap_or(0) as usize;
        if tmp_length > 0 {
            // Check if there are enough bytes to pull out id field
            if value.len() < tmp_length {
                return Err(format!(
                    "Too few bytes to create ID field: need {}, have {}",
                    tmp_length,
                    value.len()
                ));
            }

            // Checked that the bytes we require are available, now collect them
            id_bytes = value.drain(0..tmp_length).collect();

            // Convert ID from bytes to UTF-8 characters
            id_field = match from_utf8(&id_bytes) {
                Ok(id_val) => Some(id_val.to_string()),
                Err(_) => return Err("Unable to convert ID bytes to valid UTF-8".to_string()),
            };
        }

        // Collect remaining data as payload after validating length
        if value.len() < payload_length as usize {
            return Err(format!(
                "Too few bytes to create payload field: need {}, have {}",
                payload_length,
                value.len()
            ));
        }
        let payload: Vec<u8> = Vec::from_iter(value.drain(0..payload_length as usize).into_iter());

        // Succesfully built Record object from u8 slice
        Ok(NDEFRecord {
            header: header,
            type_length: type_length,
            payload_length: payload_length,
            id_length: id_length,
            record_type: type_field,
            id_field: id_field,
            payload: payload,
        })
    }
}

#[no_mangle]
pub extern "C" fn ndef_recordFromBytes(bytes: *const uint8_t, len: size_t) -> *mut NDEFRecord {
    let record_bytes: &[u8] = unsafe {
        // Confirm that the bytes pointer passed is not null to start
        assert!(!bytes.is_null());

        // Create byte slice from pointer and length given
        slice::from_raw_parts(bytes, len)
    };

    // Attempt to create Record object from converted slice of u8
    match NDEFRecord::try_from(record_bytes) {
        Ok(record) => Box::into_raw(Box::new(record)),
        Err(err) => {
            eprintln!("{}", err);
            ptr::null_mut()
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn ndef_from_bytes_valid_text() {
        use super::{tnf::TypeNameFormat, NDEFRecord, RecordHeader};
        use std::convert::TryFrom;

        // Record Header
        // - Message Begin (1b), Message End (1b), Last chunk (0b), Short Record (1b)
        // - ID Length not present (0b), NFC Forum Well Known Type TNF (0b001)
        let header_byte: u8 = 0xd1;

        // Type length
        // - Payload type field is 1 octet long (single "T" char)
        // - No ID Length
        let type_length: u8 = 0x01;

        // Payload length
        // - 13 octet (character) long payload
        // - SR flag set
        let payload_length: u8 = 0x13;

        // Well Known Type - Text (ASCII "T")
        let well_known_type: u8 = 0x54;

        // Text encoding information
        // - UTF-8 (1b), RFU (always 0b), IANA language code "en-US" length = 5 (0b00101)
        let text_flag: u8 = 0x85;

        // ISO/IANA language code "en-US" encoded in US-ASCII
        let lang_code_bytes = vec![0x65, 0x6e, 0x2d, 0x55, 0x53];

        // UTF-8 encoded text payload ("Hello, World")
        let encoded_text = vec![
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21,
        ];

        let mut test_bytes: Vec<u8> = vec![
            header_byte,
            type_length,
            payload_length,
            well_known_type,
            text_flag,
        ];

        // Extend test_bytes with language code and UTF-8 encoded bytes
        test_bytes.extend(lang_code_bytes);
        test_bytes.extend(encoded_text);

        let expected_header = RecordHeader {
            mb: true,
            me: true,
            cf: false,
            sr: true,
            il: false,
            tnf: TypeNameFormat::WellKnown,
        };

        let record = match NDEFRecord::try_from(test_bytes.as_slice()) {
            Ok(record) => record,
            Err(err) => panic!(err),
        };

        // Get encoding and country code from payload
        let mut text_payload: String;
        if ((record.payload[0] >> 7) & 0x01) != 0 {
            // Decoding UTF-8
            // Ignore language code length and language code, last 5 bits are the ISO/IANA language code bytes length
            let lang_code_len = record.payload[0] & 0x17;

            // Ignore UTF-x/RFU/IANA code length byte and then ISO/IANA language code bytes
            let num_ignore_bytes = (1 + lang_code_len) as usize;

            // Extract text payload from UTF-8 bytes
            text_payload = match String::from_utf8(record.payload[num_ignore_bytes..].to_vec()) {
                Ok(txt) => txt,
                Err(err) => panic!("{}", err),
            };
        } else {
            // Decoding UTF-16 is not supported currently
            panic!("Oh frick, we can't deal with UTF-16");
            // text_payload = match String::from_utf16(record.payload.into()) {
            //     Ok(txt) => txt,
            //     Err(err) => panic!("{}", err),
            // };
        }

        assert_eq!(record.id_length, None);
        assert_eq!(record.id_field, None);
        assert_eq!(record.header, expected_header);
        assert_eq!(record.payload_length, 19);
        assert_eq!(record.record_type, "T");
        assert_eq!(record.type_length, 1);
        assert_eq!(text_payload, "Hello, World!");
    }
}
