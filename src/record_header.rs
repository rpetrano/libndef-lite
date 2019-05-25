use crate::tnf::TypeNameFormat;
use std::convert::Into;

/// # NDEF Record Header
#[derive(Debug, PartialEq)]
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
        RecordHeader {
            mb: (value & 0x80) != 0,
            me: (value & 0x40) != 0,
            cf: (value & 0x20) != 0,
            sr: (value & 0x10) != 0,
            il: (value & 0x08) != 0,
            tnf: TypeNameFormat::from(value & 0x07)
        }
    }
}

impl Into<u8> for RecordHeader {
    fn into(self) -> u8 {
        let mut byte: u8 = 0x00;
        if self.mb { byte |= 0x80; }
        if self.me { byte |= 0x40; }
        if self.cf { byte |= 0x20; }
        if self.sr { byte |= 0x10; }
        if self.il { byte |= 0x08; }
        byte |= self.tnf as u8;

        byte
    }
}

#[cfg(test)]
mod tests {
    use super::{RecordHeader, TypeNameFormat};

    #[test]
    fn new_record_header_simple() {
        let new_byte: u8 = 0xff;
        let expected = RecordHeader {
            mb: true,
            me: true,
            cf: true,
            sr: true,
            il: true,
            tnf: TypeNameFormat::from(7u8)
        };
        let record_header = RecordHeader::new(new_byte);
        
        assert_eq!(expected, record_header);
    }

    #[test]
    fn new_record_header_short() {
        let new_byte: u8 = 0xd1;
        let expected = RecordHeader {
            mb: true,
            me: true,
            cf: false,
            sr: true,
            il: false,
            tnf: TypeNameFormat::WellKnown
        };
        let record_header = RecordHeader::new(new_byte);
        
        assert_eq!(expected, record_header);
    }

    #[test]
    fn header_to_byte() {
        let header = RecordHeader {
            mb: true,
            me: true,
            cf: false,
            sr: true,
            il: false,
            tnf: TypeNameFormat::WellKnown
        };
        let byte: u8 = header.into();
        assert_eq!(byte, 0b11010001);
    }
}