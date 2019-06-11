#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <deque>
#include <memory>
#include <stdexcept>
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

  /*! Implementation of the C++20 span helper
  * 
  * \tparam ElementType type of the object stored in the array
  */
  template<typename ElementType>
    class span {
    public:
      /// Constructor that creates unique_ptr to empty array
      /// \param size Size of the new array to create
      span(uint64_t size)
        : elements(std::make_unique<ElementType[]>(size)), numElements(size) { }

      // Constructor from unique_ptr to element array
      /// \param ptr Unique pointer to start of data array
      /// \param size Size of the data array. \note ptr must be moved for this constructor
      span(std::unique_ptr<ElementType[]> ptr, uint64_t size)
        : elements(std::move(ptr)), numElements(size) { }

      /// Constructor from pointer. Must be a heap-based array
      /// \param ptr Pointer to start of data array
      /// \param size Size of the data array
      /// \bug Unable to pass pointer to static array
      span(ElementType *ptr, uint64_t size) 
        : elements(std::unique_ptr<ElementType[]>{ptr}), numElements(size) { }
      
      // Accessors

      /// \param idx Index within data array to attempt to retrieve value
      /// \throws std::out_of_range if idx is an invalid index (>= size)
      /// \return ElementType reference at index
      ElementType& at(const uint64_t idx) const;

      /// Provides access to the data array via pointer
      /// \return std::unique_ptr to start of array
      ElementType* data() { return elements.get(); }

      // Operators

      /// \note Simply a wrapper around at()
      /// \param idx Index within data array to attempt to retrieve
      /// \throws std::out_of_range if idx is an invalid index (>= size)
      /// \return ElementType reference at index
      constexpr ElementType& operator[](const uint64_t idx) const { return at(idx); }

      // Observers

      /// Access the size of the array
      /// \return uint64_t of size of array
      uint64_t size() const { return numElements; }

      /// Helper function to check if the span is empty
      /// \return bool indicating whether the array is empty
      bool empty() const { return numElements == 0; }

    private:
      std::unique_ptr<ElementType[]> elements;
      uint64_t numElements;
    };

  /// Implementation of span::at
  template<typename T>
    T& span<T>::at(uint64_t idx) const {
      if (idx >= numElements) {
        throw std::out_of_range("invalid index " + std::to_string(idx) + " for span");
      }

      return elements.get()[idx];
    }
}

#endif // UTIL_H