#ifndef DYNAMIC_ALLOCATOR_HPP
#define DYNAMIC_ALLOCATOR_HPP

#include "Allocators/Allocator.hpp"

#include "Exceptions.hpp"

namespace MyStd
{

template<typename T>
class DynamicAllocator final : public IAllocator<T>
{
    char* data_;
    size_t size_;
    size_t capacity_;

public:
    
    DynamicAllocator() : data_(nullptr), size_(0), capacity_(0) {}
    DynamicAllocator(size_t size);
    DynamicAllocator(size_t size, const T& value);
    DynamicAllocator(const DynamicAllocator& other);

    DynamicAllocator& operator=(const DynamicAllocator& other);

    T* data() noexcept override;

    const T* data()   const noexcept override;
    size_t size()     const noexcept override;
    size_t capacity() const noexcept override;

    void size(const size_t newSize) noexcept override;

    void free() noexcept override;
    void realloc(size_t newCapacity) override;
    void realloc(size_t newCapacity, const T& value) override;
    void dtorElements(size_t from, size_t to) noexcept override;
    
    AllocatorProxyValue<T> operator[](size_t pos) override;
    const T& operator[](size_t pos) const override;

    void swap(DynamicAllocator& other) noexcept;

    ~DynamicAllocator();
};

// --------------------------Implementation-----------------------------------

template<typename T>
void DynamicAllocator<T>::swap(DynamicAllocator& other) noexcept
{
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
DynamicAllocator<T>::DynamicAllocator(size_t size) : size_(0), capacity_(size)
{
    data_ = allocateMem<T>(capacity_);
}
    
template<typename T>
DynamicAllocator<T>::DynamicAllocator(size_t size, const T& value) : size_(0), capacity_(size)
{
    data_ = allocateMem<T>(capacity_);

    try
    {
        copyData(*this, 0, capacity_, value);
    }
    catch (ExceptionWithReason& e)
    {
        free();

        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::AllocatorCtorErr,
            "Can't copy into allocated memory in dynamic allocator",
            std::move(e)
        );
    }
    catch(...)
    {
        free();
        throw;
    }    
}

template<typename T>
DynamicAllocator<T>::DynamicAllocator(const DynamicAllocator& other) : size_(0), capacity_(other.capacity_)
{
    data_ = allocateMem<T>(capacity_);
    try
    {
        copyData(*this, 0, reinterpret_cast<T*>(other.data_), other.size_);
    }
    catch (ExceptionWithReason& e)
    {
        free();

        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::AllocatorCtorErr,
            "Can't copy into allocated memory in dynamic allocator",
            std::move(e)
        );
    }
    catch(...)
    {
        free();
        throw;
    }  
}

template<typename T>
DynamicAllocator<T>& DynamicAllocator<T>::operator=(const DynamicAllocator& other)
{
    DynamicAllocator<T> tmp{other};
    swap(tmp);

    return *this;
}

template<typename T>
T* DynamicAllocator<T>::data() noexcept
{
    return reinterpret_cast<T*>(data_);
}

template<typename T>
const T* DynamicAllocator<T>::data() const noexcept
{
    return reinterpret_cast<const T*>(data_);
}

template<typename T>
size_t DynamicAllocator<T>::size() const noexcept
{
    return size_;
}

template<typename T>
size_t DynamicAllocator<T>::capacity() const noexcept
{
    return capacity_;
}

template<typename T>
void DynamicAllocator<T>::size(const size_t newSize) noexcept
{
    size_ = newSize;
}

template<typename T>
void DynamicAllocator<T>::free() noexcept
{
    dtorElements(0, size_);
    delete [] data_;
}

template<typename T>
void DynamicAllocator<T>::realloc(size_t newCapacity)
{
    DynamicAllocator<T> tmp{newCapacity};

    copyData(tmp, 0, reinterpret_cast<T*>(data_), std::min(size_, newCapacity));

    swap(tmp);
}

template<typename T>
void DynamicAllocator<T>::realloc(size_t newCapacity, const T& value)
{
    DynamicAllocator<T> tmp{newCapacity, value};

    copyData(tmp, 0, reinterpret_cast<T*>(data_), std::min(size_, newCapacity));

    swap(tmp);
}

template<typename T>
void DynamicAllocator<T>::dtorElements(size_t fromPos, size_t to) noexcept
{
    T* typedData = reinterpret_cast<T*>(data_);
    for (size_t pos = fromPos; pos < to; ++pos)
    {
        typedData[pos].~T();
    }

    size_ -= to - fromPos;
}

template<typename T>
AllocatorProxyValue<T> DynamicAllocator<T>::operator[](size_t pos)
{
    AllocatorProxyValue<T> proxy{reinterpret_cast<T*>(data_), size_, capacity_, pos};
    return proxy;
}

template<typename T>
const T& DynamicAllocator<T>::operator[](size_t pos) const
{
    return reinterpret_cast<const T*>(data_)[pos];
}

template<typename T>
DynamicAllocator<T>::~DynamicAllocator()
{
    free();
}

} // namespace MyStd

#endif // DYNAMIC_ALLOCATOR_HPP