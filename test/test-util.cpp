#include <deque>
#include <vector>

#include "doctest.hpp"
#include "ndef-lite/util.hpp"

using namespace util;
using namespace std;

TEST_CASE("pop_front returns correct value (uint8_t)")
{
  // Hard coding values woo yay...
  std::deque<uint8_t> values{ 0, 42, 24 };

  // Ensure there are 3 values in the queue initially
  REQUIRE(values.size() == 3);

  // Pop front all values (3 times), confirming expected
  REQUIRE(pop_front(values) == 0);
  REQUIRE(pop_front(values) == 42);
  REQUIRE(pop_front(values) == 24);

  // Ensure there are 0 values in the queue at the end
  REQUIRE(values.size() == 0);
}

TEST_CASE("pop_front returns correct value (int32_t)")
{
  // Hard coding values woo yay...
  std::deque<int32_t> values{ 0, 42, 24 };

  // Ensure there are 3 values in the queue initially
  REQUIRE(values.size() == 3);

  // Pop front all values (3 times), confirming expected
  REQUIRE(pop_front(values) == 0);
  REQUIRE(pop_front(values) == 42);
  REQUIRE(pop_front(values) == 24);

  // Ensure there are 0 values in the queue at the end
  REQUIRE(values.size() == 0);
}

TEST_CASE("drain_deque returns correct values (uint8_t)")
{
  // Hard coding values woo yay...
  deque<uint8_t> values{ 0, 42, 24 };
  vector<uint8_t> expected{ 0, 42, 24 };

  // Ensure there are 3 values in the queue initially
  REQUIRE(values.size() == 3);

  // Drain all 3 values, expecting the returned vector to be equal
  REQUIRE(drain_deque(values, 3) == expected);

  // Ensure there are 0 values in the queue at the end
  REQUIRE(values.size() == 0);
}

TEST_CASE("uint32FromBEBytes creates appropriate uint32 value from 4 byte array")
{
  uint8_t values[4]{ 0x42, 0x24, 0x00, 0x00 };
  uint32_t expectedVal = 0x42240000;

  // Convert bytes to uint32
  uint32_t converted = uint32FromBEBytes(values);
  REQUIRE(converted == expectedVal);
}

TEST_CASE("assertHasValues no throw, number of elements in deque > n")
{
  deque<uint8_t> values{ 0, 42, 24 };
  REQUIRE_NOTHROW(assertHasValues(values, 2, "unit test"));
}

TEST_CASE("assertHasValues no throw, number of elements in deque == n")
{
  deque<uint8_t> values{ 0, 42, 24 };
  REQUIRE_NOTHROW(assertHasValues(values, 3, "unit test"));
}

TEST_CASE("assertHasValues throws NDEFException, number of elements in deque < n")
{
  deque<uint8_t> values{ 0, 42, 24 };
  REQUIRE_THROWS_WITH(assertHasValues(values, 4, "unit test"),
                      "Too few elements in queue for unit test field: require 4 have 3");
}

TEST_CASE("assertHasValue no throw, number of elements in deque > 1")
{
  deque<uint8_t> values{ 0, 42, 24 };
  REQUIRE_NOTHROW(assertHasValue(values, "unit test"));
}

TEST_CASE("assertHasValue no throw, number of elements in deque == n")
{
  deque<uint8_t> values{ 0, 42, 24 };
  REQUIRE_NOTHROW(assertHasValue(values, "unit test"));
}

TEST_CASE("assertHasValue throws NDEFException, empty deque")
{
  deque<uint8_t> values{};
  REQUIRE_THROWS_WITH(assertHasValue(values, "unit test"),
                      "Too few elements in queue for unit test field: require 1 have 0");
}
