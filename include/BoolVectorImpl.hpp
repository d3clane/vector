#ifndef BOOL_VECTOR_IMPL_HPP
#define BOOL_VECTOR_IMPL_HPP

#include <cassert>
#include <algorithm>

#include "BoolVector.hpp"
#include "CommonVectorFuncs.hpp"

#include "Errors.hpp"
#include "Exceptions.hpp"

namespace MyStd
{

namespace 
{

size_t getBlock(size_t size) { return size >> 3; }
size_t getShift(size_t size) { return size & 7; }

void setBit(uint8_t* data, size_t pos, bool value)
{
    size_t block = getBlock(pos);
    size_t shift = getShift(pos);

    data[block] = value ? (data[block] | (1u << shift)) : (data[block] & ~(1u << shift));
}

bool getBit(uint8_t* data, size_t pos)
{
    size_t block = getBlock(pos);
    size_t shift = getShift(pos);

    return (data[block] & (1u << shift));
}

size_t getNeededSize(size_t size)
{
    return (size + __CHAR_BIT__ - 1) / __CHAR_BIT__;
}

template<Allocator AllocatorType>
void copyData(uint8_t* data, uint8_t* from, size_t size)
{
    try
    {
        for (size_t i = 0; i < size; ++i)
        {
            setBit(data, i, getBit(from, i)); // TODO: could be more efficient
        }
    }
    catch (ExceptionWithReason& exception)
    {
        delete [] data;
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while copying vector",
            std::move(exception)
        );
    }
    catch (...)
    {
        delete [] data;
        throw;
    }
}

template<Allocator AllocatorType>
void copyData(uint8_t* data, size_t size, const bool value)
{
    try
    {
        for (size_t i = 0; i < size; ++i)
        {
            setBit(data, i, value); // TODO: could be more efficient
        }
    }
    catch (ExceptionWithReason& exception)
    {
        delete [] data;
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while copying vector",
            std::move(exception)
        );
    }
    catch (...)
    {
        delete [] data;
        throw;
    }
}

} // namespace anon

template<Allocator AllocatorType>
void Vector<bool, AllocatorType>::ProxyValue::operator=(const bool value)
{
    *data_ = value ? (*data_ | mask_) : (*data_ & ~mask_);
}

template<Allocator AllocatorType>
Vector<bool, AllocatorType>::ProxyValue::operator bool() const
{
    return (*data_ & mask_);
}

template<Allocator AllocatorType>
Vector<bool, AllocatorType>::Vector() noexcept : data_(nullptr), size_(0), capacity_(0) {}

template<Allocator AllocatorType>
Vector<bool, AllocatorType>::Vector(size_t size, const bool value)
    : data_(nullptr), size_(size), capacity_(size)
{
    data_ = reinterpret_cast<uint8_t*>(allocateMemoryInBytes<AllocatorType>(getNeededSize(size)));

    copyData<AllocatorType>(data_, size_, value);
}

template<Allocator AllocatorType>
Vector<bool, AllocatorType>::Vector(Vector<bool, AllocatorType>&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
{
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template<Allocator AllocatorType>
Vector<bool, AllocatorType>& Vector<bool, AllocatorType>::operator=(Vector<bool, AllocatorType>&& other) noexcept
{
    data_       = other.data_;
    size_       = other.size_;
    capacity_   = other.capacity_;
    other.data_ = 0;

    return *this;
}

template<Allocator AllocatorType>
Vector<bool, AllocatorType>::~Vector()
{
    delete [] data_;
}

template<Allocator AllocatorType>
typename Vector<bool, AllocatorType>::ProxyValue Vector<bool, AllocatorType>::operator[](size_t pos) noexcept
{
    return ProxyValue(&data_[getBlock(pos)], 1u << getShift(pos));
}

template<Allocator AllocatorType>
size_t Vector<bool, AllocatorType>::size() const noexcept
{
    return size_;
}

template<Allocator AllocatorType>
bool Vector<bool, AllocatorType>::empty() const noexcept
{
    return size_ == 0;
}

template<Allocator AllocatorType>
void Vector<bool, AllocatorType>::pushBack(const bool value)
{
    PushResult pushResult = tryPush(value);

    if (pushResult == PushResult::Ok)
        return;

    assert(pushResult == PushResult::NeedToResize);

    Vector<bool, AllocatorType> newVector{getCapacityAfterGrowth(capacity_)};
    copyData<AllocatorType>(newVector.data_, data_, size_);
    newVector.size_ = size_;

    pushResult = newVector.tryPush(value);
    assert(pushResult == PushResult::Ok);

    std::swap(*this, newVector);
}

template<Allocator AllocatorType>
void Vector<bool, AllocatorType>::popBack() noexcept
{
    size_--;
}

// -----------------------Private--------------------------------

template<Allocator AllocatorType>
typename Vector<bool, AllocatorType>::PushResult Vector<bool, AllocatorType>::tryPush(const bool value)
{   
    if (size_ >= capacity_)
        return PushResult::NeedToResize;

    try
    {
        setBit(data_, size_, value);
        ++size_;
    }
    catch (ExceptionWithReason& exception)
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while pushing to vector",
            std::move(exception)
        );
    }

    return PushResult::Ok;
}

template<Allocator AllocatorType>
void Vector<bool, AllocatorType>::reallocMemory(size_t newSize)
{
    assert(newSize >= size_);

    uint8_t* newData = reinterpret_cast<uint8_t*>(allocateMemoryInBytes<AllocatorType>(getNeededSize(newSize)));

    copyData<AllocatorType>(newData, data_, std::min(newSize, size_));

    delete [] data_;
    data_ = newData;
    /* size_ = size_ */
    capacity_ = newSize;
}

} // namespace MyStd

#endif // BOOL_VECTOR_IMPL_HPP
