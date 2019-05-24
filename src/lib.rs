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

use libc::{uint8_t, size_t};
use std::convert::{From, TryFrom};
use std::{slice, ptr};
use std::collections::VecDeque;
use std::str::from_utf8;
use std::iter::FromIterator;

/// # Type Name Format Field types 
/// 
/// Represents 3-bit value describing record type, sets expectation for structure/content of record
#[derive(Debug)]
pub enum TypeNameFormat {
    /// Record does not contain any information
    Empty = 0x00,

    /// Well Known Data Type - Data is defined by Record Type Definition (RTD) specification from NFC Forum
    WellKnown = 0x01,

    /// MIME Media - Data is one of the data types normally found in internet communications defined in RFC 2046
    MIMEMedia = 0x02,

    /// Absolute Uniform Resource Identifier - Pointer to a resource following the RFC 3986 syntax
    AbsoluteURI = 0x03,

    /// User-defined data that relies on the format specified by the RTD specification
    External = 0x04,

    /// Unknown data type - **Type length must be set to 0**
    Unknown = 0x05,

    /// Indicates this payload is a part of a chunked record, with the type unchanged from the first chunk
    Unchanged = 0x06,

    // Not actually implemented, but the value 0x07 is reserved by the NFC Forum
    // Reserved = 0x07,
}

impl From<u8> for TypeNameFormat {
    /// Implementation of From trait for TypeNameFormat from byte
    /// 
    /// *Note:* Any unknown/unsupported fields are treated as Unknown (0x05) type as per spec
    fn from(value: u8) -> Self {
        println!("{}", value);
        match value {
            0x00 => TypeNameFormat::Empty,
            0x01 => TypeNameFormat::WellKnown,
            0x02 => TypeNameFormat::MIMEMedia,
            0x03 => TypeNameFormat::AbsoluteURI,
            0x04 => TypeNameFormat::External,
            0x05 => TypeNameFormat::Unknown,
            0x06 => TypeNameFormat::Unchanged,
            _ => TypeNameFormat::Unknown
        }
    }
}

/// # NDEF Record Header
#[derive(Debug)]
pub struct RecordHeader {
    /// Type Name Format - identifies type of content the record contains
    pub tnf: TypeNameFormat,

    /// ID Length Flag - Indicates whether ID Length Field is present
    pub il: bool,

    /// Short Record Flag - Indicates if PAYLOAD LENGTH field is 1 byte (0-255) or less
    pub sr: bool,

    /// Chunk Flag - Indicates if this the first record chunk or in the middle
    pub cf: bool,

    /// Message End Flag - Indicates if this is the last record in the message
    pub me: bool,

    /// Message Begin - Indicates if this is the start of an NDEF message
    pub mb: bool,
}

impl RecordHeader {
    /// Create a new RecordHeader object from an octet of data
    pub fn new(value: u8) -> Self {
        println!("RecordHeader: {}", value);
        RecordHeader {
            mb: ((value >> 0) & 0x80) != 0,
            me: ((value >> 1) & 0x40) != 0,
            cf: ((value >> 2) & 0x20) != 0,
            sr: ((value >> 3) & 0x10) != 0,
            il: ((value >> 4) & 0x08) != 0,
            tnf: TypeNameFormat::from((value >> 5) & 0x07)
        }
    }
}

/// NDEF record struct
#[derive(Debug)]
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
                        None => return Err(format!("Too few bytes to create payload length, needed 4 got {}", i))
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
                None => return Err("Missing ID length byte".to_string())
            };
        }

        // Check if there are enough bytes to pull out type field
        if value.len() < type_length as usize {
            return Err(
                format!("Too few bytes to create ID length field: need {}, have {}", type_length, value.len())
            );
        }
        
        // Checked that the bytes we require are available, now collect them
        let type_bytes: Vec<u8> = value.drain(0..type_length as usize).collect();

        // Create the type field from the bytes, converting them into ASCII characters after validating them
        let mut type_field = String::new();
        for byte in type_bytes.into_iter() {
            if byte < 31 || byte == 127 {
                // Invalid character, no ASCII characters [0-31] or 127
                return Err(format!("Invalid character code {} found in type field", byte).to_string());
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
                return Err(
                    format!("Too few bytes to create ID field: need {}, have {}", tmp_length, value.len())
                );
            }

            // Checked that the bytes we require are available, now collect them
            id_bytes = value.drain(0..tmp_length).collect();
            
            // Convert ID from bytes to UTF-8 characters
            id_field = match from_utf8(&id_bytes) {
                Ok(id_val) => Some(id_val.to_string()),
                Err(_) => return Err("Unable to convert ID bytes to valid UTF-8".to_string())
            };
        }

        // Collect remaining data as payload after validating length
        if value.len() < payload_length as usize {
            return Err(
                format!("Too few bytes to create payload field: need {}, have {}", payload_length, value.len())
            )
        }
        let payload: Vec<u8> = Vec::from_iter(value.drain(0..payload_length as usize).into_iter().map(|p| p as u8));

        // Succesfully built Record object from u8 slice
        Ok(
            NDEFRecord {
                header: header,
                type_length: type_length,
                payload_length: payload_length,
                id_length: id_length,
                record_type: type_field,
                id_field: id_field,
                payload: payload
            }
        )
    }
}

/// Well-Known Record - URI - [TNF Record Type `0x01`](enum.TypeNameFormat.html#variant.WellKnown)
/// 
/// Well Known Type is "U" ([Type](struct.Record.html#structfield.record_type) field will be `0x55`)
pub struct URIRecord {
    /// This field allows the uri_field to be compacted, by expressing common protocols as a 1 byte value
    /// 
    /// Table taken from NFC Forum standard
    /// 
    /// | Decimal | Hex        | Protocol                   |
    /// | ------- | ---------- | -------------------------- |
    /// |    0    | 0x00       | None - nothing prepended   |
	/// |    1    | 0x01       | http://www.                |
	/// |    2    | 0x02       | https://www.               |
	/// |    3    | 0x03       | http://                    |
	/// |    4    | 0x04       | https://                   |
	/// |    5    | 0x05       | tel:                       | 
	/// |    6    | 0x06       | mailto:                    |
	/// |    7    | 0x07       | ftp://anonymous:anonymous@ |
	/// |    8    | 0x08       | ftp://ftp.                 |
	/// |    9    | 0x09       | ftps://                    |
	/// |    10   | 0x0A       | sftp://                    |
	/// |    11   | 0x0B       | smb://                     |
	/// |    12   | 0x0C       | nfs://                     |
	/// |    13   | 0x0D       | ftp://                     |
	/// |    14   | 0x0E       | dav://                     |
	/// |    15   | 0x0F       | news:                      |
	/// |    16   | 0x10       | telnet://                  |
	/// |    17   | 0x11       | imap:                      |
	/// |    18   | 0x12       | rtsp://                    |
	/// |    19   | 0x13       | urn:                       |
	/// |    20   | 0x14       | pop:                       |
	/// |    21   | 0x15       | sip:                       |
	/// |    22   | 0x16       | sips:                      |
	/// |    23   | 0x17       | tftp:                      |
	/// |    24   | 0x18       | btspp://                   |
	/// |    25   | 0x19       | btl2cap://                 |
	/// |    26   | 0x1A       | btgoep://                  |
	/// |    27   | 0x1B       | tcpobex://                 |
	/// |    28   | 0x1C       | irdaobex://                |
	/// |    29   | 0x1D       | file://                    |
	/// |    30   | 0x1E       | urn:epc:id:                |
	/// |    31   | 0x1F       | urn:epc:tag:               |
	/// |    32   | 0x20       | urn:epc:pat:               |
	/// |    33   | 0x21       | urn:epc:raw:               |
	/// |    34   | 0x22       | urn:epc:                   |
	/// |    35   | 0x23       | urn:nfc:                   |
    /// | 36…255  | 0x24..0xFF | RFU - **Do not use**       |
    pub identifier_code: u8,

    /// This field must be encoded as UTF-8, unless the URI scheme specifies differently
    pub uri_field: &'static str,
}

#[no_mangle]
pub extern fn ndef_recordFromBytes(bytes: *const uint8_t, len: size_t) -> *mut NDEFRecord {
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