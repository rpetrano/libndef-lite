#include "recordHeader.hpp"
#include "recordType.hpp"

namespace NDEFRecordHeader {
/// Create a new NDEFRecordHeader object from an byte of data
RecordHeader fromByte(const uint8_t value)
{
  return RecordHeader{ .mb = (value & static_cast<uint8_t>(RecordFlag::MB)) != 0,
                       .me = (value & static_cast<uint8_t>(RecordFlag::ME)) != 0,
                       .cf = (value & static_cast<uint8_t>(RecordFlag::CF)) != 0,
                       .sr = (value & static_cast<uint8_t>(RecordFlag::SR)) != 0,
                       .il = (value & static_cast<uint8_t>(RecordFlag::IL)) != 0,
                       // Take last 3 bits and retrieve matching TypeName
                       .tnf = NDEFRecordType::fromByte(value & 0x07) };
}

/// Creates a byte representation of the NDEFRecordHeader object passed
uint8_t asByte(const RecordHeader& header)
{
  uint8_t byte = 0x00;

  byte |= (header.mb) ? static_cast<uint8_t>(RecordFlag::MB) : 0;
  byte |= (header.me) ? static_cast<uint8_t>(RecordFlag::ME) : 0;
  byte |= (header.cf) ? static_cast<uint8_t>(RecordFlag::CF) : 0;
  byte |= (header.sr) ? static_cast<uint8_t>(RecordFlag::SR) : 0;
  byte |= (header.il) ? static_cast<uint8_t>(RecordFlag::IL) : 0;
  byte |= NDEFRecordType::asByte(header.tnf);

  return byte;
}
} // namespace NDEFRecordHeader