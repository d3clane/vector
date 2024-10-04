#ifndef BOOL_VECTOR_IMPL_HPP
#define BOOL_VECTOR_IMPL_HPP

#include "BoolVector.hpp"
#include "VectorImpl.hpp"

namespace MyStd
{

namespace 
{

void setBit(uint8_t* data, size_t pos, bool value)
{
    size_t block = pos >> 3;
    size_t shift = pos & 7;

    data[block] = value ? (data[block] | (1 << shift)) : (data[block] & ~(1 << shift));
}

bool getBit(uint8_t* data, size_t pos)
{
    size_t block = pos >> 3;
    size_t shift = pos & 7;
    return (data[block] & (1 << shift));
}

size_t getNeededSize(size_t size)
{
    static const int bitsInByte = 8;
    return (size + bitsInByte - 1) / bitsInByte;
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

template<Allocator AllocatorType>
AllocatedMemInfo<uint8_t> allocateMemory(size_t size)
{
    auto info = allocateMemory<uint8_t, AllocatorType>(getNeededSize(size));

    return AllocatedMemInfo<uint8_t>{info.data_, 0, size};
}

template<Allocator AllocatorType>
void reallocMemory(Vector<bool, AllocatorType>& data, size_t newSize)
{
    assert(newSize >= data.size_);

    uint8_t* newData = allocateMemory<AllocatorType>(newSize).data_;

    copyData<AllocatorType>(newData, data, data + std::min(newSize, data.size_)); // TODO: в другом месте баг без мина скорее всего

    delete [] data.data_;
    data.data_ = newData;
    data.size_ = data.size_;
    data.capacity_ = newSize;
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
    : data_(nullptr), size_(0), capacity_(0)
{
    updateVectorInfo(data_, size_, capacity_, allocateMemory<uint8_t, AllocatorType>(size));

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
    return ProxyValue(&data_[pos / 8], 1 << (pos % 8));
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

} // namespace MyStd

#endif // BOOL_VECTOR_IMPL_HPP
