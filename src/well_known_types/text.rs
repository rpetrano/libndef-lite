use std::convert::TryFrom;

/// Well-Known Record - Text - [TNF Record Type `0x01`](enum.TypeNameFormat.html#variant.WellKnown)
/// 
/// Well Known Type is "T" ([Type](struct.Record.html#structfield.record_type) field will be `0x54`)
pub struct TextRecord { }

impl TryFrom<Vec<u8>> for TextRecord {
    type Error = String;

    pub fn try_from(value: Vec<u8>) -> Result<Self, Self::Error> {
        
    }
}