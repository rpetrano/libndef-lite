#ifndef SPAN_H
#define SPAN_H

template<typename ElementType>
  class span {
  public:
    span(ElementType *ptr, uint64_t size) 
      : data(ptr), size(size) { }
    ~span() = default;
    
    // Accessors
    ElementType& at(uint64_t idx);

    // Operators
    constexpr override ElementType& operator[](uint64_t idx) = at(idx);

    // Observers
    uint64_t size() const { return this.size; }
    bool empty() const { return this.size == 0; }
  private:
    ElementType *data;
    uint64_t size;
  };

template<typename T>
  T& span<T>::at(uint64_t idx) {
    
  }

#endif // SPAN_H