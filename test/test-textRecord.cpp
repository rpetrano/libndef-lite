#include <string>
#include <vector>

#include "doctest.hpp"
#include "test-constants.hpp"

#include "ndef-lite/record.hpp"

using namespace std;

TEST_CASE("Initialize text record payload")
{
  string expected_payload = "testing123";
  string expected_locale = "en-US";

  NDEFRecord record = NDEFRecord::create_text_record(expected_payload, expected_locale);
  REQUIRE(record.get_text() == expected_payload);
  REQUIRE(record.get_text_locale() == expected_locale);
}

TEST_CASE("Text from payload byte array")
{
  string expected = "Hello, World!";

  REQUIRE(NDEFRecord::get_text(valid_utf8_text_payload) == expected);
}

TEST_CASE("Text locale from byte array")
{
  string expected_locale = "en-US";

  REQUIRE(NDEFRecord::get_text_locale(valid_utf8_text_payload) == expected_locale);
}