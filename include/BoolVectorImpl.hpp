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

inline size_t getBlock(size_t size) { return size >> 3; }
inline size_t getShift(size_t size) { return size & 7; }

inline void setBit(uint8_t* data, size_t pos, bool value)
{
    size_t block = getBlock(pos);
    size_t shift = getShift(pos);

    data[block] = value ? (data[block] | (1u << shift)) : (data[block] & ~(1u << shift));
}

inline bool getBit(uint8_t* data, size_t pos)
{
    size_t block = getBlock(pos);
    size_t shift = getShift(pos);

    return (data[block] & (1u << shift));
}

inline size_t getNeededSize(size_t size)
{
    return (size + __CHAR_BIT__ - 1) / __CHAR_BIT__;
}

inline bool needToIncreaseSizeInAllocator(size_t allocatorSize, size_t bitsSize)
{   
    return bitsSize / __CHAR_BIT__ > allocatorSize;
}

inline void copyData(uint8_t* data, uint8_t* from, size_t size)
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

inline void copyData(uint8_t* data, size_t size, const bool value)
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

template<typename Allocator>
void Vector<bool, Allocator>::ProxyValue::operator=(const bool value)
{
    *data_ = value ? (*data_ | mask_) : (*data_ & ~mask_);
}

template<typename Allocator>
Vector<bool, Allocator>::ProxyValue::operator bool() const
{
    return (*data_ & mask_);
}

template<typename Allocator>
Vector<bool, Allocator>::Vector() noexcept : allocator_(), size_(0) 
{
}

template<typename Allocator>
Vector<bool, Allocator>::Vector(size_t size) : allocator_(getNeededSize(size)), size_(0)
{
}

template<typename Allocator>
Vector<bool, Allocator>::Vector(size_t size, const bool value)
    : allocator_(getNeededSize(size), value), size_(size)
{
    copyData(allocator_.data(), size_, value);
}

template<typename Allocator>
typename Vector<bool, Allocator>::ProxyValue Vector<bool, Allocator>::operator[](size_t pos) noexcept
{
    return ProxyValue(reinterpret_cast<uint8_t*>(allocator_.data()) + getBlock(pos), 1u << getShift(pos));
}

template<typename Allocator>
size_t Vector<bool, Allocator>::size() const noexcept
{
    return size_;
}

template<typename Allocator>
bool Vector<bool, Allocator>::empty() const noexcept
{
    return size_ == 0;
}

template<typename Allocator>
void Vector<bool, Allocator>::pushBack(const bool value)
{
    PushResult pushResult = tryPush(value);

    if (pushResult == PushResult::Ok)
        return;

    assert(pushResult == PushResult::NeedToResize);

    Vector<bool, Allocator> newVector{getCapacityAfterGrowth(allocator_.capacity())};

    copyData(
        reinterpret_cast<uint8_t*>(newVector.allocator_.data()), 
        reinterpret_cast<uint8_t*>(allocator_.data()), 
        size_
    );

    newVector.allocator_.size(allocator_.size());
    newVector.size_ = size_;

    pushResult = newVector.tryPush(value);
    assert(pushResult == PushResult::Ok);

    swap(newVector);
}

template<typename Allocator>
void Vector<bool, Allocator>::popBack() noexcept
{
    size_--;
}


template<typename Allocator>
typename Vector<bool, Allocator>::ProxyValue Vector<bool, Allocator>::front() noexcept
{
    return this->operator[](0);
}

template<typename Allocator>
bool Vector<bool, Allocator>::front() const noexcept
{
    return this->operator[](0);
}

template<typename Allocator>
typename Vector<bool, Allocator>::ProxyValue Vector<bool, Allocator>::back() noexcept
{
    return this->operator[](size_ - 1);
}

template<typename Allocator>
bool Vector<bool, Allocator>::back() const noexcept
{
    return this->operator[](size_ - 1);
}

template<typename Allocator>
void Vector<bool, Allocator>::swap(Vector& other)
{
    allocator_.swap(other.allocator_);
    std::swap(size_, other.size_);
}

// TODO: need to reimplement everything?? (resize, etc..)

// -----------------------Private--------------------------------

template<typename Allocator>
typename Vector<bool, Allocator>::PushResult Vector<bool, Allocator>::tryPush(const bool value)
{   
    if (needToIncreaseSizeInAllocator(allocator_.capacity(), allocator_.size()))
        return PushResult::NeedToResize;

    try
    {
        setBit(reinterpret_cast<uint8_t*>(allocator_.data()), size_, value);

        if (needToIncreaseSizeInAllocator(allocator_.size(), size_))
            allocator_.size(allocator_.size() + 1);

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

} // namespace MyStd

#endif // BOOL_VECTOR_IMPL_HPP
