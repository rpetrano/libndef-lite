#include "doctest.hpp"
#include "test-constants.hpp"

#include "ndef-lite/record.hpp"

using namespace std;

TEST_CASE("Initialize URI record payload")
{
  string expected_protocol = "http://";
  string expected_uri = "testing123.com";

  NDEFRecord record = NDEFRecord::create_uri_record(expected_protocol + expected_uri);
  REQUIRE(record.get_uri() == expected_uri);
  REQUIRE(record.get_uri_protocol() == expected_protocol);
}

TEST_CASE("Initialize short URI record payload")
{
  string expected_protocol = "ftp://";
  string expected_uri = "a.co";

  NDEFRecord record = NDEFRecord::create_uri_record(expected_protocol + expected_uri);
  REQUIRE(record.get_uri() == expected_uri);
  REQUIRE(record.get_uri_protocol() == expected_protocol);
}

TEST_CASE("URI Protocol from payload byte array")
{
  string expected_protocol = "https://www.";

  REQUIRE(NDEFRecord::get_uri_protocol(valid_https_prefix_uri_payload) == expected_protocol);
}

TEST_CASE("URI from payload byte array")
{
  string expected_uri = "google.com";

  REQUIRE(NDEFRecord::get_uri(valid_https_prefix_uri_payload) == expected_uri);
}