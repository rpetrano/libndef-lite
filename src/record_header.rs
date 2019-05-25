use crate::tnf::TypeNameFormat;

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

#[cfg(test)]
mod tests {
    #[test]
    fn new_record_header_simple() {
        use super::{RecordHeader, TypeNameFormat};

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
        use super::{RecordHeader, TypeNameFormat};

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
}