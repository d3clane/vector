#ifndef ALLOCATORS_STATIC_ALLOCATOR
#define ALLOCATORS_STATIC_ALLOCATOR

#include <cstddef>

#include "Allocators/Allocator.hpp"

namespace MyStd
{

template<typename T, size_t capacity>
class StaticAllocator final : public Allocator<T>
{
    size_t size_;
    size_t capacity_ = capacity;

    char data[capacity];
public:
    StaticAllocator() : size_(0) {}

    StaticAllocator(size_t size);
    StaticAllocator(size_t size, const T& value);
    StaticAllocator(const DynamicAllocator& other);

    StaticAllocator& operator=(const DynamicAllocator& other);

    T* data() override;

    const T* data()   const override;
    size_t size()     const override;
    size_t capacity() const override;

    void free() override;
    void realloc(size_t newCapacity) override;
    void realloc(size_t newCapacity, const T& value) override;
    void dtorElements(size_t from, size_t to) override;
    
    AllocatorProxyValue<T> operator[](size_t pos) override;
    T& operator[](size_t pos) const override;

    void swap(StaticAllocator& other);

    ~StaticAllocator() = default;
};


// ------------------Implementation-------------------------

} // namespace MyStd

#endif // ALLOCATORS_STATIC_ALLOCATOR