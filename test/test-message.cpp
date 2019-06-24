#include <vector>

#include "doctest.hpp"
#include "test-constants.hpp"

#include "ndef-lite/message.hpp"
#include "ndef-lite/record.hpp"

TEST_CASE("NDEF Message insert past end throws out_of_range")
{
  NDEFMessage message;
  NDEFRecord record;

  // Max position should be 0
  REQUIRE_THROWS_WITH(message.insert_record(record, 1), "Unable to insert record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message insert not past end does not throw")
{
  NDEFMessage message;
  NDEFRecord record;

  // Max position should be 0
  REQUIRE_NOTHROW(message.insert_record(record, 0));
}

TEST_CASE("NDEF Message remove at valid location (populated)")
{
  // Prepare message with single record
  NDEFMessage message{ NDEFRecord{} };

  REQUIRE_NOTHROW(message.remove_record(0));
}

TEST_CASE("NDEF Message remove past end (empty) throws out_of_range")
{
  NDEFMessage message;

  // Should not be allowed to remove at any position
  REQUIRE_THROWS_WITH(message.remove_record(0), "Unable to remove record. Index 0 outside of range of message");
}

TEST_CASE("NDEF Message remove past end (populated) throws out_of_range")
{
  // Prepare message with single record
  NDEFMessage message{ NDEFRecord{} };

  // Should not be allowed to remove at any position
  REQUIRE_THROWS_WITH(message.remove_record(1), "Unable to remove record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message set past end (empty) throws out_of_range")
{
  NDEFMessage message;
  NDEFRecord record;

  // Should not be allowed to set at any position
  REQUIRE_THROWS_WITH(message.set_record(record, 1), "Unable to set record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message set past end (populated) throws out_of_range")
{
  // Prepare message with single record
  NDEFMessage message{ NDEFRecord{} };
  NDEFRecord record;

  // Should not be allowed to set at any position
  REQUIRE_THROWS_WITH(message.set_record(record, 1), "Unable to set record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message valid set (empty)")
{
  // Prepare message with single record
  NDEFMessage message{ NDEFRecord{} };
  NDEFRecord record;

  REQUIRE_NOTHROW(message.set_record(record, 0));
}

TEST_CASE("Empty message invalid")
{
  NDEFMessage message;

  REQUIRE_FALSE(message.is_valid());
}

TEST_CASE("Populated NDEF Message is valid")
{
  auto valid_record = NDEFRecord::from_bytes(valid_text_record_bytes_sr);
  NDEFMessage msg{ valid_record };

  REQUIRE(msg.is_valid());
}

TEST_CASE("Valid NDEF Message from valid bytes")
{
  auto msg = NDEFMessage::from_bytes(valid_text_record_bytes_sr);

  REQUIRE(msg.is_valid());
}

TEST_CASE("Valid NDEF Message from multiple valid record bytes")
{
  auto msg_bytes = valid_text_record_bytes_sr;
  msg_bytes.insert(msg_bytes.end(), valid_text_record_bytes_sr.begin(), valid_text_record_bytes_sr.end());
  auto msg = NDEFMessage::from_bytes(msg_bytes);

  REQUIRE(msg.is_valid());
}

TEST_CASE("Invalid NDEF Message from mixed validity record bytes")
{
  auto msg_bytes = valid_text_record_bytes_sr;
  msg_bytes.insert(msg_bytes.end(), invalid_record_bytes.begin(), invalid_record_bytes.end());
  auto msg = NDEFMessage::from_bytes(msg_bytes);

  REQUIRE(msg.is_valid());
}

TEST_CASE("Expected byte sequence from valid bytes")
{
  auto valid_record = NDEFRecord::from_bytes(valid_text_record_bytes_sr);
  auto msg = NDEFMessage{ valid_record };

  REQUIRE(msg.is_valid());
  REQUIRE(msg.as_bytes() == valid_text_record_bytes_sr);
}

TEST_CASE("Expected byte sequence from multiple valid bytes")
{
  auto expected_bytes = valid_text_record_bytes_sr;
  expected_bytes.insert(expected_bytes.end(), valid_text_record_bytes_sr.begin(), valid_text_record_bytes_sr.end());
  auto valid_record = NDEFRecord::from_bytes(valid_text_record_bytes_sr);
  NDEFMessage msg{ valid_record };

  REQUIRE(msg.is_valid());
  REQUIRE(msg.as_bytes() == valid_text_record_bytes_sr);
}

TEST_CASE("Expected byte sequence from mixed validity bytes - invalid first")
{
  auto expected_bytes = valid_text_record_bytes_sr;
  expected_bytes.insert(expected_bytes.end(), invalid_record_bytes.begin(), invalid_record_bytes.end());
  NDEFMessage msg{ NDEFRecordList{
      NDEFRecord::from_bytes(invalid_record_bytes),
      NDEFRecord::from_bytes(valid_text_record_bytes_sr),
  } };

  REQUIRE_FALSE(msg.is_valid());

  // Expecting no data back
  REQUIRE(msg.as_bytes().empty());
}

TEST_CASE("Expected byte sequence from mixed validity bytes - valid first")
{
  auto expected_bytes = valid_text_record_bytes_sr;
  expected_bytes.insert(expected_bytes.end(), invalid_record_bytes.begin(), invalid_record_bytes.end());
  NDEFMessage msg;
  msg.append_record(NDEFRecord::from_bytes(valid_text_record_bytes_sr));
  msg.append_record(NDEFRecord::from_bytes(invalid_record_bytes));

  REQUIRE_FALSE(msg.is_valid());

  // Expecting only one record to be returned, that of the valid text record
  REQUIRE(msg.as_bytes().empty());
}

// Constructor tests
TEST_CASE("Valid NDEF Message from data bytes and type, no offset")
{
  auto type = NDEFRecordType::text_record_type();

  NDEFMessage msg{ valid_text_record_bytes_sr, type };

  REQUIRE(msg.is_valid());
}

TEST_CASE("Invalid NDEF Message from data bytes and Invalid type, no offset")
{
  auto type_id = NDEFRecordType::TypeID::Invalid;
  NDEFRecordType type{ type_id };

  NDEFMessage msg{ valid_text_record_bytes_sr, type };

  REQUIRE_FALSE(msg.is_valid());
}

TEST_CASE("Invalid NDEF Message from invalid data bytes, no offset")
{
  auto invalid_record = NDEFRecord::from_bytes(invalid_record_bytes);

  REQUIRE_FALSE(invalid_record.is_valid());

  NDEFMessage msg{ invalid_record };

  REQUIRE_FALSE(msg.is_valid());
}

TEST_CASE("Valid NDEF Message from single record list")
{
  NDEFRecordList records{ NDEFRecord{} };
  NDEFMessage msg{ records };

  REQUIRE(msg.is_valid());
}

TEST_CASE("Invalid NDEF Message from list with single invalid Record, no offset")
{
  NDEFRecordList invalid_records{ NDEFRecord::from_bytes(invalid_record_bytes),
                                  NDEFRecord::from_bytes(invalid_record_bytes) };

  NDEFMessage msg{ invalid_records };

  REQUIRE_FALSE(msg.is_valid());
}

TEST_CASE("Invalid NDEF Message from multiple invalid data bytes, no offset")
{
  NDEFRecordList mixed_records{ NDEFRecord::from_bytes(invalid_record_bytes),
                                NDEFRecord::from_bytes(valid_text_record_bytes_sr) };

  NDEFMessage msg{ mixed_records };

  REQUIRE_FALSE(msg.is_valid());
}