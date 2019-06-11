#include <deque>
#include <vector>

#include "catch.hpp"
#include "util.hpp"

using namespace util;

TEST_CASE( "popFront returns correct value (uint8_t)", "[popFrontUint8]" ) {
  // Hard coding values woo yay...
  std::deque<uint8_t> values { 0, 42, 24 };

  // Ensure there are 3 values in the queue initially
  REQUIRE( values.size() == 3 );

  // Pop front all values (3 times), confirming expected
  REQUIRE( popFront(values) == 0 );
  REQUIRE( popFront(values) == 42 );
  REQUIRE( popFront(values) == 24 );

  // Ensure there are 0 values in the queue at the end
  REQUIRE( values.size() == 0 );
}

TEST_CASE( "popFront returns correct value (int32_t)", "[popFrontInt32]" ) {
  // Hard coding values woo yay...
  std::deque<int32_t> values { 0, 42, 24 };

  // Ensure there are 3 values in the queue initially
  REQUIRE( values.size() == 3 );

  // Pop front all values (3 times), confirming expected
  REQUIRE( popFront(values) == 0 );
  REQUIRE( popFront(values) == 42 );
  REQUIRE( popFront(values) == 24 );

  // Ensure there are 0 values in the queue at the end
  REQUIRE( values.size() == 0 );
}

TEST_CASE( "drainDeque returns correct values (uint8_t)", "[drainDequeUint8]" ) {
  // Hard coding values woo yay...
  std::deque<uint8_t> values { 0, 42, 24 };
  std::vector<uint8_t> expected { 0, 42, 24 };

  // Ensure there are 3 values in the queue initially
  REQUIRE( values.size() == 3 );

  // Drain all 3 values, expecting the returned vector to be equal
  REQUIRE( drainDeque(values, 3) == expected );

  // Ensure there are 0 values in the queue at the end
  REQUIRE( values.size() == 0 );
}

TEST_CASE( "uint32FromBEBytes creates appropriate uint32 value from 4 byte array", "[uint32FromBEBytes]" ) {
  uint8_t values[4] { 0x42, 0x24, 0x00, 0x00 };
  uint32_t expectedVal = 0x42240000;

  // Convert bytes to uint32
  uint32_t converted = uint32FromBEBytes(values);
  REQUIRE( converted == expectedVal );
}

TEST_CASE( "assertHasValues no throw, number of elements in deque > n", "[assertHasValuesPassGT]") {
  std::deque<uint8_t> values { 0, 42, 24 };
  REQUIRE_NOTHROW( assertHasValues(values, 2, "unit test") );
}

TEST_CASE( "assertHasValues no throw, number of elements in deque == n", "[assertHasValuesPassEQ]") {
  std::deque<uint8_t> values { 0, 42, 24 };
  REQUIRE_NOTHROW( assertHasValues(values, 3, "unit test") );
}

TEST_CASE( "assertHasValues throws NDEFException, number of elements in deque < n", "[assertHasValuesThrow]") {
  std::deque<uint8_t> values { 0, 42, 24 };
  REQUIRE_THROWS_WITH( 
    assertHasValues(values, 4, "unit test"),
    "Too few elements in queue for unit test field: require 4 have 3" 
  );
}

TEST_CASE( "assertHasValue no throw, number of elements in deque > 1", "[assertHasValuePassGT]") {
  std::deque<uint8_t> values { 0, 42, 24 };
  REQUIRE_NOTHROW( assertHasValue(values, "unit test") );
}

TEST_CASE( "assertHasValue no throw, number of elements in deque == n", "[assertHasValuePassEQ]") {
  std::deque<uint8_t> values { 0, 42, 24 };
  REQUIRE_NOTHROW( assertHasValue(values, "unit test") );
}

TEST_CASE( "assertHasValue throws NDEFException, empty deque", "[assertHasValueThrowEmpty]") {
  std::deque<uint8_t> values { };
  REQUIRE_THROWS_WITH( 
    assertHasValue(values, "unit test"),
    "Too few elements in queue for unit test field: require 1 have 0" 
  );
}

TEST_CASE( "span initializes correct number of elements", "[spanInitCorrectNumElems]" ) {
  const auto expectedSpanSize = 4;
  span<uint8_t> testSpan{expectedSpanSize};
  
  REQUIRE_FALSE( testSpan.empty() );
  REQUIRE( testSpan.size() == expectedSpanSize );
}

TEST_CASE( "span is empty", "[spanIsEmpty]" ) {
  const auto expectedSpanSize = 0;
  span<uint8_t> testSpan{expectedSpanSize};
  
  REQUIRE( testSpan.empty() );
  REQUIRE( testSpan.size() == expectedSpanSize );
}

TEST_CASE( "span from unique pointer", "[spanFromUnique]" ) {
  const auto expectedSpanSize = 4;
  auto testPtr = std::make_unique<uint8_t[]>(expectedSpanSize);
  testPtr[0] = 42;
  testPtr[1] = 24;
  testPtr[2] = 21;
  testPtr[3] = 12;
  span<uint8_t> testSpan{move(testPtr), expectedSpanSize};

  REQUIRE_FALSE( testSpan.empty() );
  REQUIRE( testSpan.size() == 4 );

  REQUIRE( testSpan.at(0) == 42 );
  REQUIRE( testSpan.at(1) == 24 );
  REQUIRE( testSpan.at(2) == 21 );
  REQUIRE( testSpan.at(3) == 12 );
  
  REQUIRE( testSpan[0] == 42 );
  REQUIRE( testSpan[1] == 24 );
  REQUIRE( testSpan[2] == 21 );
  REQUIRE( testSpan[3] == 12 );
}

TEST_CASE( "editable span from unique pointer", "[editSpanFromUnique]" ) {
  const auto expectedSpanSize = 4;
  auto testPtr = std::make_unique<uint8_t[]>(expectedSpanSize);
  span<uint8_t> testSpan{move(testPtr), expectedSpanSize};

  REQUIRE_FALSE( testSpan.empty() );
  REQUIRE( testSpan.size() == 4 );

  // Set values via .at()
  testSpan.at(0) = 0;
  testSpan.at(1) = 1;
  testSpan.at(2) = 2;
  testSpan.at(3) = 3;

  REQUIRE( testSpan.at(0) == 0 );
  REQUIRE( testSpan.at(1) == 1 );
  REQUIRE( testSpan.at(2) == 2 );
  REQUIRE( testSpan.at(3) == 3 );
  
  REQUIRE( testSpan[0] == 0 );
  REQUIRE( testSpan[1] == 1 );
  REQUIRE( testSpan[2] == 2 );
  REQUIRE( testSpan[3] == 3 );

  // Set values via subscript operator
  testSpan[0] = 9;
  testSpan[1] = 8;
  testSpan[2] = 7;
  testSpan[3] = 6;

  REQUIRE( testSpan.at(0) == 9 );
  REQUIRE( testSpan.at(1) == 8 );
  REQUIRE( testSpan.at(2) == 7 );
  REQUIRE( testSpan.at(3) == 6 );
  
  REQUIRE( testSpan[0] == 9 );
  REQUIRE( testSpan[1] == 8 );
  REQUIRE( testSpan[2] == 7 );
  REQUIRE( testSpan[3] == 6 );
}

TEST_CASE( "span from temporary unique pointer", "[spanFromTempUnique]" ) {
  const auto expectedSpanSize = 4;
  span<uint8_t> testSpan{
    std::make_unique<uint8_t[]>(expectedSpanSize),
    expectedSpanSize
  };

  testSpan.at(0) = 42;
  testSpan.at(1) = 24;
  testSpan.at(2) = 21;
  testSpan.at(3) = 12;

  REQUIRE_FALSE( testSpan.empty() );
  REQUIRE( testSpan.size() == 4 );

  REQUIRE( testSpan.at(0) == 42 );
  REQUIRE( testSpan.at(1) == 24 );
  REQUIRE( testSpan.at(2) == 21 );
  REQUIRE( testSpan.at(3) == 12 );
  
  REQUIRE( testSpan[0] == 42 );
  REQUIRE( testSpan[1] == 24 );
  REQUIRE( testSpan[2] == 21 );
  REQUIRE( testSpan[3] == 12 );
}

TEST_CASE( "span from raw pointer", "[spanRawPointer]" ) {
  const auto expectedSpanSize = 2;
  uint8_t *values = new uint8_t[expectedSpanSize]{ 0, 1 };
  span<uint8_t> testSpan{values, expectedSpanSize};
  
  REQUIRE_FALSE( testSpan.empty() );
  REQUIRE( testSpan.size() == expectedSpanSize );

  REQUIRE( testSpan.at(0) == 0 );
  REQUIRE( testSpan.at(1) == 1 );
}

TEST_CASE( "span to pointer", "[spanToPointer]" ) {
  const auto expectedSpanSize = 2;
  uint8_t *values = new uint8_t[expectedSpanSize]{ 0, 1 };
  span<uint8_t> testSpan{values, expectedSpanSize};
  
  REQUIRE_FALSE( testSpan.empty() );
  REQUIRE( testSpan.size() == expectedSpanSize );

  REQUIRE( testSpan.at(0) == 0 );
  REQUIRE( testSpan.at(1) == 1 );
}