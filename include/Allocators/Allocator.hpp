#ifndef ALLOCATORS_ALLOCATOR_HPP
#define ALLOCATORS_ALLOCATOR_HPP

#include <cstddef>

namespace MyStd
{

template<typename T>
class AllocatorProxyValue
{
    T* data_;
    size_t& size_;
    size_t& capacity_;
    size_t pos_;

public:
    AllocatorProxyValue(T* data, size_t& size, size_t& capacity, size_t pos) : 
        data_(data), size_(size), capacity_(capacity), pos_(pos){}

    virtual T& operator=(const T& value);
    virtual ~AllocatorProxyValue() = default;

    virtual operator T&();
};

template<typename T>
class Allocator
{
public:
    virtual ~Allocator() = default;

    virtual T* data() = 0;
    virtual const T* data() const = 0;
    virtual size_t size() const = 0;
    virtual size_t capacity() const = 0;

    virtual void free() = 0;
    virtual void realloc(size_t newCapacity) = 0;
    virtual void realloc(size_t newCapacity, const T& value) = 0;
    
    virtual void dtorElements(size_t from, size_t to) = 0;

    virtual AllocatorProxyValue<T> operator[](size_t pos) = 0;
    virtual T& operator[](size_t pos) const = 0;
};

/* swap(Allocator& a, Allocator& b) */

// ----------------------Implementation----------------------

template<typename T>
void constructInMemory(T* memory, const T& value)
{
    new (memory) T(value);
}

template<typename T>
void copyToMemory(T* memory, const T& value)
{
    *memory = value;
}

template<typename T>
T& AllocatorProxyValue<T>::operator=(const T& value)
{
    // NO CHECKS
    if (pos_ >= size_)
    {
        constructInMemory(data_ + pos_, value);
        ++size_;
    }
    else 
        copyToMemory(data_ + pos_, value);
    
    return data_[pos_];
}

template<typename T>
AllocatorProxyValue<T>::operator T&()
{
    return data_[pos_];
}

} // namespace MyStd

#endif // ALLOCATORS_ALLOCATOR_HPP