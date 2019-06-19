#include "catch.hpp"

#include "ndef/message.hpp"
#include "ndef/record.hpp"

TEST_CASE("NDEF Message insert past end throws out_of_range", "messageInsertPastEndThrows")
{
  NDEFMessage message;
  NDEFRecord record;

  // Max position should be 0
  REQUIRE_THROWS_WITH(message.insert_record(record, 1), "Unable to insert record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message insert not past end does not throw", "messageInsertValidDoesNotThrow")
{
  NDEFMessage message;
  NDEFRecord record;

  // Max position should be 0
  message.insert_record(record, 0);
}

TEST_CASE("NDEF Message remove past end (empty) throws out_of_range", "messageRemovePastEndThrows-empty")
{
  NDEFMessage message;

  // Should not be allowed to remove at any position
  REQUIRE_THROWS_WITH(message.remove_record(0), "Unable to remove record. Index 0 outside of range of message");
}

TEST_CASE("NDEF Message remove past end (populated) throws out_of_range", "messageRemovePastEndThrows-populated")
{
  // Prepare message with single record
  NDEFMessage message{ NDEFRecord{} };

  // Should not be allowed to remove at any position
  REQUIRE_THROWS_WITH(message.remove_record(1), "Unable to remove record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message set past end (empty) throws out_of_range", "messageSetPastEndThrows-empty")
{
  NDEFMessage message;
  NDEFRecord record;

  // Should not be allowed to set at any position
  REQUIRE_THROWS_WITH(message.set_record(record, 1), "Unable to set record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message set past end (populated) throws out_of_range", "messageSetPastEndThrows-populated")
{
  // Prepare message with single record
  NDEFMessage message{ NDEFRecord{} };
  NDEFRecord record;

  // Should not be allowed to set at any position
  REQUIRE_THROWS_WITH(message.set_record(record, 1), "Unable to set record. Index 1 outside of range of message");
}

TEST_CASE("NDEF Message valid set (empty)", "messageSetValid")
{
  // Prepare message with single record
  NDEFMessage message{ NDEFRecord{} };
  NDEFRecord record;

  // Should not be allowed to set at any position
  message.set_record(record, 0);
}