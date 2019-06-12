#ifndef NDEF_H
#define NDEF_H

#include <string>
#include <vector>

#include "recordHeader.hpp"
#include "util.hpp"

// NDEF Text record is encoded with either UTF8 or UTF16, this is part of a byte set in the first bits of the payload
enum class RecordTextCodec { UTF8 = 0x00, UTF16 = 0x80 };

/// NDEF Record struct
class NDEFRecord {
public:
  NDEFRecord();
  NDEFRecord(const std::vector<uint8_t>& data, const NDEFRecordType::Type type = NDEFRecordType::Type::Empty,
             uint offset = 0, bool chunked = false);
  NDEFRecord(NDEFRecordHeader::RecordHeader header, std::string typeField, std::string idField,
             std::vector<uint8_t> payload);
  ~NDEFRecord() = default;

  /// \return vector of uint8 byte values
  std::vector<uint8_t> asBytes() const;

  /// \param bytes array of bytes (uint8_t) that will be used to attempt to create a Record object
  /// \return Record object created from bytes
  static NDEFRecord fromBytes(uint8_t bytes[], size_t len);

  /// \param bytes vector of bytes (uint8_t) that will be used to attempt to create a NDEFRecord object
  /// \return NDEFRecord object created from bytes
  static NDEFRecord fromBytes(std::vector<uint8_t> bytes);

  // Accessors/Mutators
  void constexpr setId(const std::string& id) { this->idField = id; }
  std::string id() const { return this->idField; }

  void constexpr setChunked(bool flag) { this->header.cf = flag; }
  bool constexpr isChunked() const { return this->header.cf; }

  bool constexpr isShort() const { return (this->payload.size() < 256); }
  bool constexpr isEmpty() const { return (this->header.tnf == NDEFRecordType::Type::Empty); }
  bool constexpr isValid() const { return (this->header.tnf != NDEFRecordType::Type::Invalid); }

private:
  // NDEF Record Fields
  /// NDEF Record header object
  NDEFRecordHeader::RecordHeader header;

  /// Specifies length of payload type in octets
  //   uint8_t typeLength;

  /// Specifies length of payload in octets
  ///
  /// If `SR` flag is set in the record header then this value will be 1 byte, otherwise it will be a 32-bit value
  //   uint32_t payloadLength;

  /// Specifies length of ID field in octets
  ///
  /// Only included if the `IL` flag is set in the record header, otherwise omitted entirely
  //   uint8_t idLength;

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

  /// Payload - A slice of octets, the length of which is declared in #payloadLength
  std::vector<uint8_t> payload;
};

#endif // NDEF_H