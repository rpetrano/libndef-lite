//! # Type Name Format Field
//! 
//! This code provides helper functions and whatnot for the NFC Forum standard Type Name Format field

/// # Type Name Format Field types 
/// 
/// Represents 3-bit value describing record type, sets expectation for structure/content of record
#[derive(Debug, Clone, PartialEq)]
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

#[cfg(test)]
mod tests {
    #[test]
    fn tnf_try_from_u8() {
        use super::TypeNameFormat;

        let tests: Vec<(u8, TypeNameFormat)> = vec![
            (0x00, TypeNameFormat::Empty),
            (0x01, TypeNameFormat::WellKnown),
            (0x02, TypeNameFormat::MIMEMedia),
            (0x03, TypeNameFormat::AbsoluteURI),
            (0x04, TypeNameFormat::External),
            (0x05, TypeNameFormat::Unknown),
            (0x06, TypeNameFormat::Unchanged),
            (0x07, TypeNameFormat::Unknown),
            (0x45, TypeNameFormat::Unknown),
        ];

        for case in tests {
            let tnf = TypeNameFormat::from(case.0);
            assert_eq!(tnf, case.1);
        }
    }
}