#ifndef ALLOCATORS_ALLOCATOR_HPP
#define ALLOCATORS_ALLOCATOR_HPP

#include <cstddef>

#include "Exceptions.hpp"

namespace MyStd
{

template<typename T>
class AllocatorProxyValue final
{
    T* data_;
    size_t& size_;
    size_t& capacity_;
    size_t pos_;

public:
    AllocatorProxyValue(T* data, size_t& size, size_t& capacity, size_t pos) : 
        data_(data), size_(size), capacity_(capacity), pos_(pos){}

    T& operator=(const T& value);

    operator T&() noexcept;
    operator T*() noexcept;
};

template<typename T>
class IAllocator
{
public:
    virtual ~IAllocator() = default;

    virtual T* data() noexcept = 0;
    virtual const T* data() const noexcept = 0;
    virtual size_t size() const noexcept = 0;
    virtual size_t capacity() const noexcept = 0;

    virtual void size(const size_t newSize) noexcept = 0;

    virtual void free() = 0;
    virtual void realloc(size_t newCapacity) = 0;
    virtual void realloc(size_t newCapacity, const T& value) = 0;
    
    virtual void dtorElements(size_t from, size_t to) = 0;

    virtual AllocatorProxyValue<T> operator[](size_t pos) = 0;
    virtual const T& operator[](size_t pos) const = 0;
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
AllocatorProxyValue<T>::operator T&() noexcept
{
    return data_[pos_];
}

template<typename T>
AllocatorProxyValue<T>::operator T*() noexcept
{
    return data_ + pos_;
}

#define CATCH_EXCEPTION(ERROR, REASON)  \
    catch (ExceptionWithReason& exception) \
    { \
        allocator.dtorElements(fromPos, dataPos - 1); \
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(ERROR, REASON, std::move(exception)); \
    } \
    catch (...) \
    { \
        allocator.dtorElements(fromPos, dataPos - 1); \
        throw; \
    }

template<typename T, typename Allocator>
void copyData(Allocator& allocator, const size_t fromPos, size_t count, const T& value)
{
    size_t dataPos = fromPos;
    try
    {
        for (dataPos = fromPos; dataPos < count + fromPos; ++dataPos)
        {
            allocator[dataPos] = value;
        }
    }
    CATCH_EXCEPTION(StdErrors::VectorCtorErr, "Failed to copy elements while copying allocator");
}

template<typename T, typename Allocator>
void copyData(Allocator& allocator, const size_t fromPos, T* otherData, size_t count)
{
    size_t dataPos = fromPos;
    try
    {
        for (size_t otherDataPos = fromPos; otherDataPos < count; ++otherDataPos)
        {
            allocator[dataPos] = otherData[otherDataPos];
            dataPos++;
        }
    }
    CATCH_EXCEPTION(StdErrors::VectorCtorErr, "Failed to copy elements while copying allocator");
}

#undef CATCH_EXCEPTION

template<typename T>
char* allocateMem(size_t size)
{
    char* data = nullptr;
    try
    {
        data = new char[size * sizeof(T)];
    }
    catch(std::bad_alloc& exception)
    {
        delete [] data;
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::MemAllocErr,
            "Failed to allocate memory in allocator",
            {}
        );
    }
    catch(...)
    {
        delete [] data;
        throw;
    }

    return data;
}

} // namespace MyStd

#endif // ALLOCATORS_ALLOCATOR_HPP