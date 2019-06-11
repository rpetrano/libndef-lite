#include "catch.hpp"
#include "ndef.hpp"
#include "util.hpp"

std::vector<uint8_t> validTextRecordBytes() {
    // Record Header
    // - Message Begin (1b), Message End (1b), Last chunk (0b), Short Record (1b)
    // - ID Length not present (0b), NFC Forum Well Known Type TNF (0b001)
    uint8_t header_byte { 0xd1 };

    // Type length
    // - Payload type field is 1 octet long (single "T" char)
    // - No ID Length
    uint8_t type_length { 0x01 };

    // Payload length
    // - 19 octet (character) long payload
    // - SR flag set
    uint8_t payload_length { 0x13 };

    // Well Known Type - Text (ASCII "T")
    uint8_t well_known_type { 0x54 };

    // Text encoding information
    // - UTF-8 (1b), RFU (always 0b), IANA language code "en-US" length = 5 (0b00101)
    uint8_t text_flag { 0x85 };

    // ISO/IANA language code "en-US" encoded in US-ASCII
    std::vector<uint8_t> langCodeBytes { 0x65, 0x6e, 0x2d, 0x55, 0x53 };

    // UTF-8 encoded text payload ("Hello, World")
    std::vector<uint8_t> encodedText {
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21,
    };

    std::vector<uint8_t> testBytes {
        header_byte,
        type_length,
        payload_length,
        well_known_type,
        text_flag,
    };

    // Extend test_bytes with language code and UTF-8 encoded bytes
    testBytes.insert(testBytes.end(), langCodeBytes.begin(), langCodeBytes.end());
    testBytes.insert(testBytes.end(), encodedText.begin(), encodedText.end());

    return testBytes;
}

TEST_CASE( "Create valid NDEF Record from known valid bytes",  "[ndefFromBytesValidText]" ) {
  using namespace ndef;
  using namespace std;

  vector<uint8_t> testBytes = validTextRecordBytes();

  auto expectedHeader = RecordHeader::RecordHeader {
    .mb = true,
    .me = true,
    .cf = false,
    .sr = true,
    .il = false,
    .tnf = TypeNameFormat::Type::WellKnown,
  };

  Record record = recordFromBytes(testBytes);

  CHECK( record.idLength == 0 );
  CHECK( record.idField == "" );
  CHECK( record.header == expectedHeader );
  CHECK( record.payloadLength == 19 );
  CHECK( record.typeLength == 1 );
  CHECK( record.recordType == "T" );

  // Get encoding and country code from payload
  string textPayload;
  REQUIRE( ((record.payload[0] >> 7) & 0x01) != 0 );
  
  // Decoding UTF-8
  // Ignore language code length and language code, last 5 bits are the ISO/IANA language code bytes length
  uint8_t langCodeLen = record.payload[0] & 0x1f;

  // Ignore UTF-x/RFU/IANA code length byte and then ISO/IANA language code bytes
  size_t numIgnoreBytes = 1 + langCodeLen;

  INFO("Num ignored:" << numIgnoreBytes);
  INFO("Payload: " << string(record.payload.begin(), record.payload.end()));

  // Extract text payload from UTF-8 bytes
  textPayload = string(record.payload.begin() + numIgnoreBytes, record.payload.end());

  CHECK( textPayload == "Hello, World!" );
}

TEST_CASE( "Valid NDEF Record returns valid bytes", "[bytesFromRecord]" ) {
  using namespace ndef;
  using namespace std;
  using namespace util;

  // Create known valid text record
  vector<uint8_t> testBytes = validTextRecordBytes();
  Record record = recordFromBytes(testBytes);

  // Convert record to bytes
  vector<uint8_t> bytes = recordToBytes(record);

  REQUIRE( bytes.size() == testBytes.size() );
  for (size_t i = 0; i < testBytes.size(); i++) {
    auto byte = bytes.at(i);
    auto testByte = testBytes.at(i);
    INFO(i);
    CHECK( testBytes.at(i) == bytes[i] );
  }
}