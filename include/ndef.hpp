#ifndef NDEF_H
#define NDEF_H

#include <string>
#include <vector>

#include "recordHeader.hpp"
#include "span.hpp"

namespace ndef {
  /// NDEF Record struct
  struct Record {
    /// NDEF Record header object
    RecordHeader::RecordHeader header;

    /// Specifies length of payload type in octets
    uint8_t typeLength;

    /// Specifies length of payload in octets
    ///
    /// If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 32-bit value
    uint32_t payloadLength;

    /// Specifies length of ID field in octets
    ///
    /// Only included if the `IL` flag is set in the record header, otherwise omitted entirely
    uint8_t idLength;

    /// Specifies record type. Must follow the structure, encoding, and format implied by the value of the TNF field. If
    /// a NDEF record is received with a supported TNF field but unknown TYPE field value, the type identifier will be
    /// treated as if the TNF field were `0x05` (Unknown)
    ///
    /// For example, if the TNF is set to 0x01 the record type might be `T` to indicate a text message, `U` for a URI
    /// message. If the TNF is set 0x02 then the record type might be one of "text/json", "text/plain", "image/png",
    /// etc.
    std::string recordType;

    /// ID field
    ///
    /// Only included if the `IL` flag is set in the record header and the ID_LENGTH field is > 0
    std::string idField;

    /// Payload - A slice of octets, the length of which is declared in
    /// #payloadLength
    std::vector<uint8_t> payload;
  };

  /// \param bytes array of bytes (uint8_t) that will be used to attempt to create a Record object
  /// \return Record object created from bytes
  Record recordFromBytes(uint8_t bytes[]);

  /// \param bytes vector of bytes (uint8_t) that will be used to attempt to create a Record object
  /// \return Record object created from bytes
  Record recordFromBytes(std::vector<uint8_t> bytes);

  /// \param reference to ndef::Record object that is being converted into bytes
  /// \return span of uint8 byte values
  span<uint8_t> recordToBytes(Record record);
}

#endif // NDEF_H