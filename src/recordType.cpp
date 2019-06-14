#include "recordType.hpp"
#include "recordHeader.hpp"

NDEFRecordType::NDEFRecordType(NDEFRecordType::TypeID id, const std::string& name) : typeName(name), typeID(id)
{
  // An empty record has no type name by the NDEF standard
  if (id == NDEFRecordType::TypeID::Empty) {
    typeName.clear();
  }
}

/// Creates an NDEFRecordType object from a vector of bytes starting at offset
NDEFRecordType NDEFRecordType::fromBytes(std::vector<uint8_t> data, size_t offset)
{
  // Current position within vector
  int byteOffset = 0;
  size_t numBytes = data.size() - offset;

  // Have to have at least 2 bytes of data to create record header
  if (numBytes < 2) {
    return NDEFRecordType{ NDEFRecordType::TypeID::Invalid };
  }

  // Have the required minimum of 2 bytes, create the record with remaining data after offset
  std::vector<uint8_t> remaining{ data.begin() + offset, data.end() };

  // Current byte being worked with
  uint8_t byte;

  // 1st byte - Header (flags and TNF)
  byte = remaining.at(byteOffset++);

  // TNF field is low 3 bits
  uint8_t tnf = byte & 0x07;
  bool hasIDField = byte & static_cast<uint8_t>(RecordFlag::IL);
  bool shortRecord = byte & static_cast<uint8_t>(RecordFlag::SR);

  // 2nd byte - Type length
  uint8_t typeLength = remaining.at(byteOffset++);

  // 1 or 4 bytes depending on SR flag - payload length (skipped)
  byteOffset += shortRecord ? 1 : 4;

  // 0 or 1 byte depending on IL flag - ID length (skipped)
  byteOffset += hasIDField ? 1 : 0;

  // Type field
  std::string typeName;
  for (auto i = 0; i < typeLength; i++) {
    typeName += remaining.at(byteOffset + i);
  }

  return NDEFRecordType{ static_cast<NDEFRecordType::TypeID>(tnf), typeName };
}

NDEFRecordType NDEFRecordType::textRecordType() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "T" }; }
NDEFRecordType NDEFRecordType::uriRecordType() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "U" }; }
NDEFRecordType NDEFRecordType::smartPosterRecordType()
{
  return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "Sp" };
}
NDEFRecordType NDEFRecordType::genericControlRecordType()
{
  return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "Gc" };
}

NDEFRecordType NDEFRecordType::spActionRecordType()
{
  return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "act" };
}
NDEFRecordType NDEFRecordType::spSizeRecordType() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "s" }; }
NDEFRecordType NDEFRecordType::spTypeRecordType() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "t" }; }

NDEFRecordType NDEFRecordType::gcTargetRecordType() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "t" }; }
NDEFRecordType NDEFRecordType::gcActionRecordType() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "t" }; }
NDEFRecordType NDEFRecordType::gcDataRecordType() { return NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, "d" }; }
