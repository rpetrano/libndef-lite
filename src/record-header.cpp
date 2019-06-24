#include "ndef-lite/record-header.hpp"
#include "ndef-lite/record-type.hpp"

/// Create a new NDEFRecordHeader object from an byte of data
NDEFRecordHeader NDEFRecordHeader::from_byte(const uint8_t value)
{
  return NDEFRecordHeader{
    // Take last 3 bits and retrieve matching TypeName
    .tnf = static_cast<NDEFRecordType::TypeID>(value & 0x07),
    .il = (value & static_cast<uint8_t>(RecordFlag::IL)) != 0,
    .sr = (value & static_cast<uint8_t>(RecordFlag::SR)) != 0,
    .cf = (value & static_cast<uint8_t>(RecordFlag::CF)) != 0,
    .me = (value & static_cast<uint8_t>(RecordFlag::ME)) != 0,
    .mb = (value & static_cast<uint8_t>(RecordFlag::MB)) != 0,
  };
}

/// Creates a byte representation of the NDEFRecordHeader object passed
uint8_t NDEFRecordHeader::asByte()
{
  uint8_t byte = 0x00;

  byte |= (this->mb) ? static_cast<uint8_t>(RecordFlag::MB) : 0;
  byte |= (this->me) ? static_cast<uint8_t>(RecordFlag::ME) : 0;
  byte |= (this->cf) ? static_cast<uint8_t>(RecordFlag::CF) : 0;
  byte |= (this->sr) ? static_cast<uint8_t>(RecordFlag::SR) : 0;
  byte |= (this->il) ? static_cast<uint8_t>(RecordFlag::IL) : 0;
  byte |= static_cast<uint8_t>(this->tnf);

  return byte;
}