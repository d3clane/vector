#ifndef ALLOCATORS_STATIC_ALLOCATOR
#define ALLOCATORS_STATIC_ALLOCATOR

#include <cstddef>

#include "Allocators/Allocator.hpp"

namespace MyStd
{

template<typename T, size_t initCapacity>
class StaticAllocator final : public IAllocator<T>
{
    size_t size_ = 0;
    size_t capacity_ = initCapacity;

    char data_[initCapacity * sizeof(T)];
public:
    StaticAllocator() : size_(0) {}

    StaticAllocator(size_t size);
    StaticAllocator(size_t size, const T& value);
    StaticAllocator(const StaticAllocator& other);

    StaticAllocator& operator=(const StaticAllocator& other);

    T* data() noexcept override;

    const T* data()   const noexcept override;
    size_t size()     const noexcept override;
    size_t capacity() const noexcept override;

    void free() override;
    void realloc(size_t newCapacity) override;
    void realloc(size_t newCapacity, const T& value) override;
    void dtorElements(size_t from, size_t to) override;
    
    AllocatorProxyValue<T> operator[](size_t pos) override;

    const T& operator[](size_t pos) const override;

    void swap(StaticAllocator& other);

    ~StaticAllocator();
};


// ------------------Implementation-------------------------

template<typename T, size_t initCapacity>
void StaticAllocator<T, initCapacity>::swap(StaticAllocator& other)
{
    copyData(*this, 0, reinterpret_cast<T*>(other.data_), other.size_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template<typename T, size_t initCapacity>
StaticAllocator<T, initCapacity>::StaticAllocator(size_t size) : size_(0)
{
    if (size > capacity_)
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorOnStackNotEnoughMemory,
            "Can't allocate [SIZE] elements on stack, not enough memory",
            {}
        );
    }
}

template<typename T, size_t initCapacity>
StaticAllocator<T, initCapacity>::StaticAllocator(size_t size, const T& value) : StaticAllocator(size)
{
    try
    {
        copyData(*this, 0, size, value);
    }
    catch (ExceptionWithReason& e)
    {
        free();

        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::AllocatorCtorErr,
            "Can't copy into allocated memory in static allocator",
            std::move(e)
        );
    }
    catch(...)
    {
        free();
        throw;
    }    
}

template<typename T, size_t initCapacity>
StaticAllocator<T, initCapacity>::StaticAllocator(const StaticAllocator& other)
{
    assert(capacity_ == other.capacity_);

    try
    {
        copyData(*this, 0, reinterpret_cast<const T*>(other.data_), other.size_);
        size_ = other.size_;
    }
    catch (ExceptionWithReason& e)
    {
        free();

        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::AllocatorCtorErr,
            "Can't copy into allocated memory in static allocator",
            std::move(e)
        );
    }
    catch(...)
    {
        free();
        throw;
    }  
}

template<typename T, size_t initCapacity>
StaticAllocator<T, initCapacity>& StaticAllocator<T, initCapacity>::operator=(const StaticAllocator& other)
{
    StaticAllocator<T, initCapacity> tmp{other};
    swap(tmp);

    return *this;
}

template<typename T, size_t initCapacity>
T* StaticAllocator<T, initCapacity>::data() noexcept
{
    return reinterpret_cast<T*>(data_);
}

template<typename T, size_t initCapacity>
const T* StaticAllocator<T, initCapacity>::data() const noexcept
{
    return reinterpret_cast<const T*>(data_);
}

template<typename T, size_t initCapacity>
size_t StaticAllocator<T, initCapacity>::size() const noexcept
{
    return size_;
}

template<typename T, size_t initCapacity>
size_t StaticAllocator<T, initCapacity>::capacity() const noexcept
{
    return capacity_;
}

template<typename T, size_t initCapacity>
void StaticAllocator<T, initCapacity>::free()
{
    dtorElements(0, size_);
}

template<typename T, size_t initCapacity>
void StaticAllocator<T, initCapacity>::realloc(size_t newCapacity)
{
    StaticAllocator<T, initCapacity> tmp{newCapacity};

    copyData(tmp, 0, reinterpret_cast<T*>(data_), std::min(size_, newCapacity));

    swap(tmp);
}

template<typename T, size_t initCapacity>
void StaticAllocator<T, initCapacity>::realloc(size_t newCapacity, const T& value)
{
    StaticAllocator<T, initCapacity> tmp{newCapacity, value};

    copyData(tmp, 0, reinterpret_cast<T*>(data_), std::min(size_, newCapacity));

    swap(tmp);
}

template<typename T, size_t initCapacity>
void StaticAllocator<T, initCapacity>::dtorElements(size_t fromPos, size_t to)
{
    T* typedData = reinterpret_cast<T*>(data_);
    for (size_t pos = fromPos; pos < to; ++pos)
    {
        typedData[pos].~T();
    }

    size_ -= to - fromPos;
}

template<typename T, size_t initCapacity>
AllocatorProxyValue<T> StaticAllocator<T, initCapacity>::operator[](size_t pos)
{
    // std::cout << (1)
    AllocatorProxyValue<T> proxy{reinterpret_cast<T*>(data_), size_, capacity_, pos};
    return proxy;
}

template<typename T, size_t initCapacity>
const T& StaticAllocator<T, initCapacity>::operator[](size_t pos) const
{
    return reinterpret_cast<const T*>(data_)[pos];
}

template<typename T, size_t initCapacity>
StaticAllocator<T, initCapacity>::~StaticAllocator()
{
    std::cout << "BEGIN DTOR\n";
    free();
    std::cout << "END DTOR\n";
}

} // namespace MyStd

#endif // ALLOCATORS_STATIC_ALLOCATOR