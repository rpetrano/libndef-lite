#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <deque>
#include <vector>

#include "exceptions.hpp"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

namespace util {
  /// Helper function to pop front item from deque and return it
  /// \note Assumes there is a value in the queue to actually pop, so length checks must be done ahead of time
  /// \tparam T is type of items in queue, the first of which will be returned
  /// \param queue queue of items to pop front item from
  /// \return first item in queue
  template<typename T>
  inline T popFront(std::deque<T> &queue) {
    // Get front byte value and then remove it from queue
    auto byte = queue.front();
    queue.pop_front();

    return byte;
  }

  /// Helper function to pop multiple bytes from deque
  /// \note Assumes there are enough values in the queue to pop, so length checks must be done ahead of time
  /// \tparam T is type of items in queue, the first n of which will be returned
  /// \param queue queue of items to pop items from
  /// \param n number of items to pop from queue
  /// \return vector of values from queue
  template<typename T>
  inline std::vector<T> drainDeque(std::deque<T> &queue, size_t n) {
    // Store values that have been "popped"
    std::vector<T> retVals { queue.begin(), queue.begin() + n };

    // Remove values from queue
    queue.erase(queue.begin(), queue.begin() + n);

    return retVals;
  }

  /// Helper function to convert an array of 4 bytes in Big Endian order to uint32
  /// \param bytes 4 byte array (8 bit unsigned int) in big endian order to be converted
  /// \return uint32 in little endian order
  constexpr inline uint32_t uint32FromBEBytes(uint8_t bytes[4]) {
    return static_cast<uint32_t>(
      bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3] << 0
    );
  }

  /// Confirms that the queue passed has at least n values available, throwing an exception if not
  /// \tparam T type that the queue holds
  /// \param queue queue of elements of type T to have length checked on
  /// \param n minimum number of elements to confirm queue has
  /// \param item string of item these bytes will be used for. Used to create error message.
  /// \throws NDEFException if n >= number of elements in queue
  template<typename T>
  void assertHasValues(std::deque<T> queue, size_t n, std::string item) {
    if (queue.size() < n) {
      throw NDEFException(
        "Too few elements in queue for " + item + " field: " +
        "require " + std::to_string(n) + " have " + std::to_string(queue.size())
      );
    }

    // Number of elements >= n, no exception tossing today boys
    return;
  }

  /// Confirms that the queue passed has at least 1 value available, throwing an exception if not.
  /// \note Wrapper around assertHasValues
  /// \tparam T type that the queue holds
  /// \param item string of item this byte will be used for. Used to create error message.
  /// \param queue queue of elements of type T to have length checked on
  /// \throws NDEFException if the queue is empty
  template<typename T>
  void assertHasValue(std::deque<T> queue, std::string item) {
    // That's all we need to know
    if (queue.size() > 0) return;

    // Oh brother, there are 0 elements in the queue...
    throw NDEFException(
        "Too few elements in queue for " + item + " field: require 1 have 0"
      );
  }
}

#endif // UTIL_H