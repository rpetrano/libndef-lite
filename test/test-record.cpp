#include "doctest.hpp"
#include "test-constants.hpp"

#include "ndef-lite/record-header.hpp"
#include "ndef-lite/record.hpp"
#include "ndef-lite/util.hpp"

using namespace std;

TEST_CASE("Create valid short NDEF Record from known valid bytes array")
{
  uint8_t data_array[valid_text_record_bytes_sr.size()];
  std::copy(valid_text_record_bytes_sr.begin(), valid_text_record_bytes_sr.end(), data_array);

  NDEFRecord record = NDEFRecord::from_bytes(data_array, valid_text_record_bytes_sr.size());

  CHECK(record.id().length() == 0);
  CHECK(record.id() == "");
  CHECK(record.is_short());
  CHECK_FALSE(record.is_chunked());
  CHECK(record.payload_length() == 19);
  CHECK(record.type().name().length() == 1);
  CHECK(record.type().name() == "T");

  // Get encoding and country code from payload
  string text_payload;
  REQUIRE(((record.payload()[0] >> 7) & 0x01) != 0);

  // Decoding UTF-8
  // Ignore language code length and language code, last 5 bits are the ISO/IANA language code bytes length
  uint8_t langCodeLen = record.payload()[0] & 0x1f;

  // Ignore UTF-x/RFU/IANA code length byte and then ISO/IANA language code bytes
  size_t numIgnoreBytes = 1 + langCodeLen;

  // Extract text payload from UTF-8 bytes
  auto payloadStart = record.payload().begin();
  advance(payloadStart, numIgnoreBytes);
  text_payload = string{ payloadStart, record.payload().end() };
  INFO("Num ignored:" << numIgnoreBytes);
  INFO("Payload: " << text_payload);

  REQUIRE(text_payload == "Hello, World!");
}

TEST_CASE("Create valid short NDEF Record from known valid bytes")
{
  NDEFRecord record = NDEFRecord::from_bytes(valid_text_record_bytes_sr);

  CHECK(record.id().length() == 0);
  CHECK(record.id() == "");
  CHECK(record.is_short());
  CHECK_FALSE(record.is_chunked());
  CHECK(record.payload_length() == 19);
  CHECK(record.type().name().length() == 1);
  CHECK(record.type().name() == "T");

  // Get encoding and country code from payload
  string text_payload;
  REQUIRE(((record.payload()[0] >> 7) & 0x01) != 0);

  // Decoding UTF-8
  // Ignore language code length and language code, last 5 bits are the ISO/IANA language code bytes length
  uint8_t lang_code_len = record.payload()[0] & 0x1f;

  // Ignore UTF-x/RFU/IANA code length byte and then ISO/IANA language code bytes
  size_t num_ignore_bytes = 1 + lang_code_len;

  // Extract text payload from UTF-8 bytes
  auto payload_start = record.payload().begin();
  advance(payload_start, num_ignore_bytes);
  text_payload = string{ payload_start, record.payload().end() };
  INFO("Num ignored:" << num_ignore_bytes);
  INFO("Payload: " << text_payload);

  REQUIRE(text_payload == "Hello, World!");
}

TEST_CASE("Valid short NDEF Record returns valid bytes")
{
  using namespace util;

  // Create known valid text record
  NDEFRecord record = NDEFRecord::from_bytes(valid_text_record_bytes_sr);

  // Convert record to bytes
  vector<uint8_t> bytes = record.as_bytes();

  REQUIRE(bytes.size() == valid_text_record_bytes_sr.size());
  for (size_t i = 0; i < valid_text_record_bytes_sr.size(); i++) {
    CHECK(valid_text_record_bytes_sr.at(i) == bytes[i]);
  }
}

TEST_CASE("Valid non-short NDEF Record from known valid bytes array")
{
  NDEFRecord record = NDEFRecord::from_bytes(valid_text_record_bytes_nosr);

  CHECK(record.id().length() == 0);
  CHECK(record.id() == "");
  CHECK_FALSE(record.is_short());
  CHECK_FALSE(record.is_chunked());
  CHECK(record.payload_length() == 263);
  CHECK(record.type().name().length() == 1);
  CHECK(record.type().name() == "T");

  // Get encoding and country code from payload
  string text_payload;
  REQUIRE(((record.payload()[0] >> 7) & 0x01) != 0);

  // Decoding UTF-8
  // Ignore language code length and language code, last 5 bits are the ISO/IANA language code bytes length
  uint8_t lang_code_len = record.payload()[0] & 0x1f;

  // Ignore UTF-x/RFU/IANA code length byte and then ISO/IANA language code bytes
  size_t num_ignore_bytes = 1 + lang_code_len;

  // Extract text payload from UTF-8 bytes
  auto payload_start = record.payload().begin();
  advance(payload_start, num_ignore_bytes);
  text_payload = string{ payload_start, record.payload().end() };
  INFO("Num ignored:" << num_ignore_bytes);
  INFO("Payload: " << text_payload);

  REQUIRE(text_payload == rand_string_257);
}

TEST_CASE("Valid short NDEF Record from known valid bytes array")
{
  NDEFRecord record = NDEFRecord::from_bytes(valid_text_record_bytes_sr);

  CHECK(record.id().length() == 0);
  CHECK(record.id() == "");
  CHECK(record.is_short());
  CHECK_FALSE(record.is_chunked());
  CHECK(record.payload_length() == 19);
  CHECK(record.type().name().length() == 1);
  CHECK(record.type().name() == "T");

  // Get encoding and country code from payload
  string text_payload;
  REQUIRE(((record.payload()[0] >> 7) & 0x01) != 0);

  // Decoding UTF-8
  // Ignore language code length and language code, last 5 bits are the ISO/IANA language code bytes length
  uint8_t lang_code_len = record.payload()[0] & 0x1f;

  // Ignore UTF-x/RFU/IANA code length byte and then ISO/IANA language code bytes
  size_t num_ignore_bytes = 1 + lang_code_len;

  // Extract text payload from UTF-8 bytes
  auto payload_start = record.payload().begin();
  advance(payload_start, num_ignore_bytes);
  text_payload = string{ payload_start, record.payload().end() };
  INFO("Num ignored:" << num_ignore_bytes);
  INFO("Payload: " << text_payload);

  REQUIRE(text_payload == "Hello, World!");
}

TEST_CASE("Too few source bytes throws NDEFException")
{
  std::vector<uint8_t> short_bytes{ 0x10, 0xc0 };

  REQUIRE_THROWS_WITH(NDEFRecord::from_bytes(short_bytes), "Invalid number of octets, must have at least 4");
}

TEST_CASE("Valid NDEF Record returns correct ID")
{
  auto test_bytes = valid_text_record_bytes_sr_id;

  NDEFRecord record = NDEFRecord::from_bytes(test_bytes);

  CHECK(record.id().length() == 4);
  CHECK(record.id() == "test");
  CHECK(record.is_short());
  CHECK_FALSE(record.is_chunked());
  CHECK(record.payload_length() == 19);
  CHECK(record.type().name().length() == 1);
  CHECK(record.type().name() == "T");

  // Get encoding and country code from payload
  string text_payload;
  REQUIRE(((record.payload()[0] >> 7) & 0x01) != 0);

  // Decoding UTF-8
  // Ignore language code length and language code, last 5 bits are the ISO/IANA language code bytes length
  uint8_t lang_code_len = record.payload()[0] & 0x1f;

  // Ignore UTF-x/RFU/IANA code length byte and then ISO/IANA language code bytes
  size_t num_ignore_bytes = 1 + lang_code_len;

  // Extract text payload from UTF-8 bytes
  auto payload_start = record.payload().begin();
  advance(payload_start, num_ignore_bytes);
  text_payload = string{ payload_start, record.payload().end() };
  INFO("Num ignored:" << num_ignore_bytes);
  INFO("Payload: " << text_payload);

  REQUIRE(text_payload == "Hello, World!");
}

TEST_CASE("Non-ASCII character in type field throws")
{
  auto test_bytes = valid_text_record_bytes_sr;

  // Change type field to invalid ASCII character 31 ([0-31,127] invalid)
  test_bytes[3] = 0x1f;

  REQUIRE_THROWS_WITH(NDEFRecord::from_bytes(test_bytes), "Invalid character code 31 found in type field");
}

TEST_CASE("Non-ASCII character in type field throws during marshalling to bytes")
{
  auto record = NDEFRecord::from_bytes(valid_text_record_bytes_sr);

  // Purposefully mess up type field
  record.set_type(NDEFRecordType{ NDEFRecordType::TypeID::WellKnown, string{ 0x1f } });

  REQUIRE_THROWS_WITH(record.as_bytes(), "Invalid type field character with code 31");
}

TEST_CASE("Valid non-short NDEF Record returns correct bytes")
{
  using namespace util;

  // Create known valid text record
  NDEFRecord record = NDEFRecord::from_bytes(valid_text_record_bytes_nosr);

  // Convert record to bytes
  vector<uint8_t> bytes = record.as_bytes();

  REQUIRE(bytes.size() == valid_text_record_bytes_nosr.size());
  for (size_t i = 0; i < valid_text_record_bytes_nosr.size(); i++) {
    CHECK(valid_text_record_bytes_nosr.at(i) == bytes[i]);
  }
}

TEST_CASE("Valid NDEF Record with ID field return correct bytes")
{
  using namespace util;

  auto test_bytes = valid_text_record_bytes_sr_id;

  // Create known valid text record
  NDEFRecord record = NDEFRecord::from_bytes(test_bytes);

  // Convert record to bytes
  vector<uint8_t> bytes = record.as_bytes();

  REQUIRE(bytes.size() == test_bytes.size());
  for (size_t i = 0; i < test_bytes.size(); i++) {
    CHECK(test_bytes.at(i) == bytes[i]);
  }
}

TEST_CASE("NDEF Record with Empty type and payload data doesn't validate")
{
  // Defaults to type ID Empty and empty payload
  NDEFRecord record;

  // Will call validate automatically
  record.set_payload(valid_text_record_bytes_sr);

  REQUIRE(record.type().id() == NDEFRecordType::TypeID::Unknown);
}