#include <string>
#include <vector>

#include "doctest.hpp"
#include "test-constants.hpp"

#include "ndef-lite/record-type.hpp"

TEST_CASE("Record Type constructed matches passed values")
{
  std::string test_name = "U";
  auto test_type = NDEFRecordType::TypeID::WellKnown;
  NDEFRecordType type{ test_type, test_name };

  REQUIRE(type.id() == test_type);
  REQUIRE(type.name() == test_name);
}

TEST_CASE("Record Type empty type has empty name")
{
  std::string expected_name = "";
  auto expected_type = NDEFRecordType::TypeID::Empty;
  NDEFRecordType type{ NDEFRecordType::TypeID::Empty, "RemoveMe" };

  REQUIRE(type.id() == expected_type);
  REQUIRE(type.name() == expected_name);
}

TEST_CASE("Record Type invalid_record_type")
{
  auto type = NDEFRecordType::invalid_record_type();

  REQUIRE(type.id() == NDEFRecordType::TypeID::Invalid);
  REQUIRE(type.name() == "");
}

TEST_CASE("Record Type text_record_type")
{
  auto type = NDEFRecordType::text_record_type();

  REQUIRE(type.id() == NDEFRecordType::TypeID::WellKnown);
  REQUIRE(type.name() == "T");
}

TEST_CASE("Record Type uri_record_type")
{
  auto type = NDEFRecordType::uri_record_type();

  REQUIRE(type.id() == NDEFRecordType::TypeID::WellKnown);
  REQUIRE(type.name() == "U");
}

TEST_CASE("Record Type from_bytes TNF too large is set to Unknown")
{
  auto bytes = valid_text_record_bytes_sr;

  // Set TNF field to be too large (0x07, largest possible value in raw bytes is RFU and invalid)
  bytes[0] |= 0x07;

  auto type = NDEFRecordType::from_bytes(bytes);

  REQUIRE(type.id() == NDEFRecordType::TypeID::Unknown);
  REQUIRE(type.name() == "T");
}

TEST_CASE("Invalid Record Type from_bytes too few bytes")
{
  std::vector<uint8_t> bytes{ 0x00 };

  auto type = NDEFRecordType::from_bytes(bytes);

  REQUIRE(type.id() == NDEFRecordType::TypeID::Invalid);
  REQUIRE(type.name() == "");
}