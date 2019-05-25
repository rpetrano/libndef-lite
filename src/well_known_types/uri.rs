use crate::types::Payload;
use std::convert::{Into, TryFrom};

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
	pub uri_field: String,
}

impl Payload<URIRecord> for URIRecord {
	fn new(bytes: Vec<u8>) -> Self {
		URIRecord {
			identifier_code: 0x00,
			uri_field: "".to_string(),
		}
	}
}

impl TryFrom<Vec<u8>> for URIRecord {
	type Error = String;

	fn try_from(value: Vec<u8>) -> Result<Self, Self::Error> {
		Ok(URIRecord {
			identifier_code: 0x00,
			uri_field: "".to_string(),
		})
	}
}

impl Into<Vec<u8>> for URIRecord {
	fn into(self) -> Vec<u8> {
		vec![]
	}
}
