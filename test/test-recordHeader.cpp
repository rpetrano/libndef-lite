#include "catch.hpp"
#include "recordHeader.hpp"

TEST_CASE("Simple Record Header", "[simpleRecordHeader]")
{
  uint8_t newByte = 0xff;
  auto expected = NDEFRecordHeader{
    .mb = true,
    .me = true,
    .cf = true,
    .sr = true,
    .il = true,
    .tnf = static_cast<NDEFRecordType::TypeID>(0x07),
  };
  auto recordHeader = NDEFRecordHeader::from_byte(newByte);

  REQUIRE(expected == recordHeader);
}

TEST_CASE("Short Record header", "[shortRecordHeader]")
{
  uint8_t newByte = 0xd1;
  auto expected = NDEFRecordHeader{
    .mb = true,
    .me = true,
    .cf = false,
    .sr = true,
    .il = false,
    .tnf = NDEFRecordType::TypeID::WellKnown,
  };
  auto recordHeader = NDEFRecordHeader::from_byte(newByte);

  REQUIRE(expected == recordHeader);
}

TEST_CASE("Record Header to byte", "[headerAsByte]")
{
  auto header = NDEFRecordHeader{
    .mb = true,
    .me = true,
    .cf = false,
    .sr = true,
    .il = false,
    .tnf = NDEFRecordType::TypeID::WellKnown,
  };
  uint8_t byte = header.asByte();

  REQUIRE(byte == 0b11010001);
}
