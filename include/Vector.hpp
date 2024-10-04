#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "VectorClass.hpp"
#include "VectorIteratorImpl.hpp"

#include "Exceptions.hpp"

#include <algorithm>

namespace MyStd
{

namespace 
{

template<typename T>
struct AllocatedMemInfo
{
    T* data;
    size_t size;
    size_t capacity;

    AllocatedMemInfo(T* data, size_t size, size_t capacity) : data(data), size(size), capacity(capacity) {}
};

template<typename T, Allocator AllocatorType>
AllocatedMemInfo<T> allocateMemory(size_t size)
{
    T* data = nullptr;
    try
    {
        data = new T[size];
    }
    catch (const std::bad_alloc& e)
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::MemAllocErr,
            "Failed to allocate memory for vector",
            {}
        );
    }

    return AllocatedMemInfo<T>{data, size, size};
}

template<typename T, Allocator AllocatorType>
void copyData(
    T* data, 
    typename Vector<T, AllocatorType>::Iterator first, typename Vector<T, AllocatorType>::Iterator last
)
{
    try
    {
        size_t dataPos = 0;
        for (typename Vector<T, AllocatorType>::Iterator it = first; it != last; ++it)
        {
            data[dataPos++] = *it;
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

template<typename T, Allocator AllocatorType>
void copyData(T* data, size_t count, const T& value)
{
    try
    {
        for (size_t i = 0; i < count; ++i)
        {
            data[i] = value;
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

template<typename T, Allocator AllocatorType>
void reallocMemory(Vector<T, AllocatorType>& data, size_t newSize)
{
    assert(newSize >= data.size_);

    T* newData = allocateMemory<T, AllocatorType>(newSize);

    copyData<T, AllocatorType>(newData, data.begin(), data.begin() + data.size_);

    delete [] data.data_;
    data.data_ = newData;
    data.size_ = data.size_;
    data.capacity_ = newSize;
}

template<typename T>
void updateVectorInfo(T*& data, size_t& size, size_t& capacity, const AllocatedMemInfo<T>& info) noexcept
{
    data     = info.data;
    size     = info.size;
    capacity = info.capacity;
}

size_t getCapacityAfterGrowth(size_t capacity) noexcept
{
    static const size_t growthFactor = 2;
    static const size_t minCapacity  = 1; 
    return capacity * growthFactor + minCapacity;
}

} // namespace anon

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector() noexcept :  data_(nullptr), size_(0), capacity_(0) {}

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector(size_t size, const T& value) : size_(size), capacity_(size)
{
    updateVectorInfo(data_, size_, capacity_, allocateMemory<T, AllocatorType>(size_));

    try
    {
        for (size_t i = 0; i < size; ++i)
        {
            data_[i] = value;
        }
    }
    catch (ExceptionWithReason& exception)
    {
        delete[] data_;

        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while constructing vector",
            std::move(exception)
        );
    }
    catch (...)
    {
        delete[] data_;
        throw;
    }
}


template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector(Iterator first, Iterator last) : size_(last - first), capacity_(size_)
{
    updateVectorInfo(data_, size_, capacity_, allocateMemory<T, AllocatorType>(size_));

    copyData<T, AllocatorType>(data_, first, last);
}


template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector(Vector& other) : size_(other.size_), capacity_(other.size_)
{
    updateVectorInfo(data_, size_, capacity_, allocateMemory<T, AllocatorType>(size_));

    copyData<T, AllocatorType>(data_, other.begin(), other.end());
}

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector(Vector&& other) noexcept : size_(other.size_), capacity_(other.size_)
{
    data_ = other.data_;
    other.data_ = nullptr;
}

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>& Vector<T, AllocatorType>::operator=(const Vector& other)
{
    Vector<T, AllocatorType> copy(other);
    std::swap(*this, copy);
    return *this;
}

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>& Vector<T, AllocatorType>::operator=(Vector&& other) noexcept
{
    size_       = other.size_;
    capacity_   = other.capacity_;
    data_       = other.data_;
    other.data_ = nullptr;
    return *this;
}

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::~Vector()
{
    delete[] data_;
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::assign(size_t count, const T& value)
{
    if (count > capacity_)
        reallocMemory<T, AllocatorType>(this, count);

    copyData<T, AllocatorType>(data_, count, value);   
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::assign(Iterator first, Iterator last)
{
    size_t count = last - first;
    if (count > capacity_)
        reallocMemory<T, AllocatorType>(this, count);

    copyData<T, AllocatorType>(data_, first, last);
}


template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::Reference Vector<T, AllocatorType>::at(size_t pos)
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, AllocatorType>*>(this)->at(pos)
    );
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::ConstReference Vector<T, AllocatorType>::at(size_t pos) const
{
    if (pos >= size_)
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorIndexOutOfBounds,
            "Vector index out of bounds",
            {}
        );
    }

    return data_[pos];
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::Reference Vector<T, AllocatorType>::operator[](size_t pos) noexcept
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, AllocatorType>*>(this)->operator[](pos)
    );
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::ConstReference Vector<T, AllocatorType>::operator[](
    size_t pos
) const noexcept
{
    return data_[pos];
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::Reference Vector<T, AllocatorType>::front() noexcept
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, AllocatorType>*>(this)->front()
    );
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::ConstReference Vector<T, AllocatorType>::front() const noexcept
{
    return data_[0];
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::Reference Vector<T, AllocatorType>::back() noexcept
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, AllocatorType>*>(this)->back()
    );
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator::ConstReference Vector<T, AllocatorType>::back() const noexcept
{
    return data_[size_ - 1];
}

template<typename T, Allocator AllocatorType>
T* Vector<T, AllocatorType>::data() noexcept
{
    return const_cast<T*>(const_cast<const Vector<T, AllocatorType>*>(this)->data());
}

template<typename T, Allocator AllocatorType>
const T* Vector<T, AllocatorType>::data() const noexcept
{
    return data_;
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator Vector<T, AllocatorType>::begin() noexcept
{
    return Iterator{data_};
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator Vector<T, AllocatorType>::end() noexcept
{
    return Iterator{data_ + size_};
}

template<typename T, Allocator AllocatorType>
const typename Vector<T, AllocatorType>::Iterator Vector<T, AllocatorType>::begin() const noexcept
{
    return Iterator{data_};
}

template<typename T, Allocator AllocatorType>
const typename Vector<T, AllocatorType>::Iterator Vector<T, AllocatorType>::end() const noexcept
{
    return Iterator{data_ + size_};
}

template<typename T, Allocator AllocatorType>
bool Vector<T, AllocatorType>::empty() const noexcept
{
    return size_ == 0;
}

template<typename T, Allocator AllocatorType>
size_t Vector<T, AllocatorType>::size() const noexcept
{
    return size_;
}

template<typename T, Allocator AllocatorType>
size_t Vector<T, AllocatorType>::capacity() const noexcept
{
    return capacity_;
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::reserve(size_t newCapacity)
{
    if (newCapacity > capacity_)
        reallocMemory<T, AllocatorType>(this, newCapacity);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::shrinkToFit()
{
    reallocMemory<T, AllocatorType>(this, size_);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::clear() noexcept
{
    size_ = 0;
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::pushBack(const T& value)
{
    PushResult pushResult = tryPush(value);

    if (pushResult == PushResult::Ok)
        return;

    assert(pushResult == PushResult::NeedToResize);

    Vector<T, AllocatorType> newVector{getCapacityAfterGrowth(capacity_)};
    copyData<T, AllocatorType>(newVector.data_, begin(), end());
    newVector.size_ = size_;

    pushResult = newVector.tryPush(value);
    assert(pushResult == PushResult::Ok);

    std::swap(*this, newVector);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::popBack() noexcept
{
    size_--;
}

#if 0
template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator Vector<T, AllocatorType>::insert(Iterator pos, const T& value)
{
    
}

Iterator insert(Iterator pos, size_t count, const T& value);
Iterator insert(Iterator pos, Iterator first, Iterator last);

Iterator erase(Iterator pos);
Iterator erase(Iterator first, Iterator last);
#endif

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::resize(size_t newSize, const T& value)
{
    Vector newVector{newSize, value};
    copyData(newVector.data_, begin(), begin() + std::min(newSize, size_));

    std::swap(*this, newVector);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::swap(Vector& other)
{
    std::swap(data_, other.data_); // using my move ctor
}

// ------------------------------Private------------------------------

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::PushResult Vector<T, AllocatorType>::tryPush(const T& value)
{   
    if (size_ >= capacity_)
        return PushResult::NeedToResize;

    try
    {
        data_[size_] = value;
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

#endif