#include <deque>
#include <vector>

#include "catch.hpp"
#include "util.hpp"

using namespace util;

TEST_CASE( "popFrontByte returns correct value", "[popFrontByte]" ) {
  // Hard coding values woo yay...
  std::deque<uint8_t> values { 0, 42, 24 };

  // Ensure there are 3 values in the queue initially
  REQUIRE( values.size() == 3 );

  // Pop front all values (3 times), confirming expected
  REQUIRE( popFrontByte(values) == 0 );
  REQUIRE( popFrontByte(values) == 42 );
  REQUIRE( popFrontByte(values) == 24 );

  // Ensure there are 0 values in the queue at the end
  REQUIRE( values.size() == 0 );
}

TEST_CASE( "uint32FromBEBytes creates appropriate uint32 value from 4 byte array", "[uint32FromBEBytes]" ) {
  // Hard coding values woo yay...
  uint8_t values[4] { 0x42, 0x24, 0x00, 0x00 };
  uint32_t expectedVal = 0x42240000;

  // Convert bytes to uint32
  uint32_t converted = uint32FromBEBytes(values);
  REQUIRE( converted == expectedVal );
}