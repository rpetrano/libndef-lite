#include "ndef-lite/record-type.hpp"
#include "ndef-lite/record-header.hpp"

NDEFRecordType::NDEFRecordType(NDEFRecordType::TypeID id, const std::string& name) : type_name(name), type_id(id)
{
  // An empty record has no type name by the NDEF standard
  if (id == NDEFRecordType::TypeID::Empty) {
    type_name.clear();
  }
}

/// Creates an NDEFRecordType object from a vector of bytes starting at offset
NDEFRecordType NDEFRecordType::from_bytes(std::vector<uint8_t> data, size_t offset)
{
  // Current position within vector
  int byte_offset = 0;
  size_t num_bytes = data.size() - offset;

  // Have to have at least 2 bytes of data to create record header
  if (num_bytes < 2) {
    return NDEFRecordType::invalid_record_type();
  }

  // Have the required minimum of 2 bytes, create the record with remaining data after offset
  std::vector<uint8_t> remaining{ data.begin() + offset, data.end() };

  // Current byte being worked with
  uint8_t byte;

  // 1st byte - Header (flags and TNF)
  byte = remaining.at(byte_offset++);

  // TNF field is low 3 bits
  uint8_t tnf = byte & 0x07;
  bool has_id_field = byte & static_cast<uint8_t>(RecordFlag::IL);
  bool short_record = byte & static_cast<uint8_t>(RecordFlag::SR);

  // 2nd byte - Type length
  uint8_t type_length = remaining.at(byte_offset++);

  // 1 or 4 bytes depending on SR flag - payload length (skipped)
  byte_offset += short_record ? 1 : 4;

  // 0 or 1 byte depending on IL flag - ID length (skipped)
  byte_offset += has_id_field ? 1 : 0;

  // Ensure required number of bytes are present
  if (remaining.size() < type_length) {
    return NDEFRecordType::invalid_record_type();
  }

  // Type field
  std::string type_name;
  for (size_t i = 0; i < type_length; i++) {
    type_name += remaining.at(byte_offset + i);
  }

  auto id = static_cast<NDEFRecordType::TypeID>(tnf);

  // According to NDEF standard any unknown/unsupported TNF field values should be treated as 0x05 Unknown
  if (tnf >= static_cast<uint8_t>(NDEFRecordType::TypeID::Invalid)) {
    id = NDEFRecordType::TypeID::Unknown;
  }

  return NDEFRecordType{ id, type_name };
}

NDEFRecordType NDEFRecordType::text_record_type() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "T" }; }
NDEFRecordType NDEFRecordType::uri_record_type() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "U" }; }
NDEFRecordType NDEFRecordType::invalid_record_type() { return NDEFRecordType{ NDEFRecordType::TypeID::Invalid }; }
