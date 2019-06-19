#include "catch.hpp"
#include "recordHeader.hpp"

TEST_CASE("Simple Record Header", "[simpleRecordHeader]")
{
  uint8_t newByte = 0xff;
  auto expected = NDEFRecordHeader{
    .tnf = static_cast<NDEFRecordType::TypeID>(0x07),
    .il = true,
    .sr = true,
    .cf = true,
    .me = true,
    .mb = true,
  };
  auto recordHeader = NDEFRecordHeader::from_byte(newByte);

  REQUIRE(expected == recordHeader);
}

TEST_CASE("Short Record header", "[shortRecordHeader]")
{
  uint8_t newByte = 0xd1;
  auto expected = NDEFRecordHeader{
    .tnf = NDEFRecordType::TypeID::WellKnown,
    .il = false,
    .sr = true,
    .cf = false,
    .me = true,
    .mb = true,
  };
  auto recordHeader = NDEFRecordHeader::from_byte(newByte);

  REQUIRE(expected == recordHeader);
}

TEST_CASE("Record Header to byte", "[headerAsByte]")
{
  auto header = NDEFRecordHeader{
    .tnf = NDEFRecordType::TypeID::WellKnown,
    .il = false,
    .sr = true,
    .cf = false,
    .me = true,
    .mb = true,
  };
  uint8_t byte = header.asByte();

  REQUIRE(byte == 0b11010001);
}
