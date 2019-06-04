#ifndef SPAN_H
#define SPAN_H

#include <memory>
#include <stdexcept>

/*! Implementation of the C++20 span helper
 * 
 * \tparam ElementType type of the object stored in the array
 */
template<typename ElementType>
  class span {
  public:
    /// \param size Size of the new array to create
    span(uint64_t size) {
      this->size = size;
      this->elements = std::shared_ptr<ElementType[]>(new ElementType[size]);
    }

    /// \param ptr Pointer to start of data array
    /// \param size Size of the data array
    span(ElementType *ptr, uint64_t size) 
      : elements(ptr), numElements(size) { }
    ~span() = default;
    
    // Accessors

    /// \param idx Index within data array to attempt to retrieve value
    /// \throws std::out_of_range if idx is an invalid index (>= size)
    /// \return ElementType reference at index
    ElementType& at(uint64_t idx);

    /// Provides access to the data array via pointer
    /// \return std::unique_ptr to start of array
    ElementType* data() { return this->elements.get(); }

    // Operators

    /// \note Simply a wrapper around at()
    /// \param idx Index within data array to attempt to retrieve
    /// \throws std::out_of_range if idx is an invalid index (>= size)
    /// \return ElementType reference at index
    constexpr ElementType& operator[](const uint64_t idx) const { return at(idx); }

    // Observers

    /// Access the size of the array
    /// \return uint64_t of size of array
    uint64_t size() const { return this->numElements; }

    /// Helper function to check if the span is empty
    /// \return bool indicating whether the array is empty
    bool empty() const { return this->numElements == 0; }

  private:
    std::shared_ptr<ElementType[]> elements;
    uint64_t numElements;
  };

/// Implementation of span::at
template<typename T>
  T& span<T>::at(uint64_t idx) {
    if (idx >= this->numElements) {
      throw std::out_of_range("invalid index " + std::to_string(idx) + " for span");
    }

    return *(elements + idx);
  }

#endif // SPAN_H