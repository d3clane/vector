#ifndef VECTOR_IMPL_HPP
#define VECTOR_IMPL_HPP

#include "VectorClass.hpp"
#include "VectorIteratorImpl.hpp"
#include "CommonVectorFuncs.hpp"

#include "Exceptions.hpp"

#include <algorithm>
#include <cstdio>

namespace MyStd
{

namespace 
{

template<typename T, Allocator AllocatorType>
void copyToEmptyData(
    char* data, 
    typename Vector<T, AllocatorType>::ConstIterator first, 
    typename Vector<T, AllocatorType>::ConstIterator last
)
{
    size_t dataPos = 0;
    try
    {
        for (typename Vector<T, AllocatorType>::ConstIterator it = first; it != last; ++it)
        {
            new(&data[dataPos * sizeof(T)]) T(*it);
            dataPos++;
        }
    }
    catch (ExceptionWithReason& exception)
    {
        dtorElements(reinterpret_cast<T*>(data), 0, dataPos - 1);
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while copying vector",
            std::move(exception)
        );
    }
    catch (...)
    {
        dtorElements(reinterpret_cast<T*>(data), 0, dataPos - 1);
        throw;
    }
}

template<typename T, Allocator AllocatorType>
void copyToEmptyData(char* data, size_t count, const T& value)
{
    size_t dataPos = 0;
    try
    {
        for (dataPos = 0; dataPos < count; ++dataPos)
        {
            new(&data[dataPos * sizeof(T)]) T(value);
        }
    }
    catch (ExceptionWithReason& exception)
    {
        dtorElements(reinterpret_cast<T*>(data), 0, dataPos - 1);
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while copying vector",
            std::move(exception)
        );
    }
    catch (...)
    {
        dtorElements(reinterpret_cast<T*>(data), 0, dataPos - 1);
        throw;
    }
}

template<typename T, Allocator AllocatorType>
void tryCopyToEmptyDataElseDelete(char* data, size_t count, const T& value)
{
    try
    {
        copyToEmptyData<T, AllocatorType>(data, count, value);
    }
    catch (...)
    {
        delete [] data;
        throw;
    }
}

template<typename T, Allocator AllocatorType>
void tryCopyToEmptyDataElseDelete(
    char* data,
    typename Vector<T, AllocatorType>::ConstIterator first,
    typename Vector<T, AllocatorType>::ConstIterator last
)
{
    try
    {
        copyToEmptyData<T, AllocatorType>(data, first, last);
    }
    catch(...)
    {
        delete [] data;
        throw;
    }
}

template<typename T, Allocator AllocatorType>
void rewriteData(char* data, size_t count, const T& value)
{
    size_t dataPos = 0;

    T* dataTyped = reinterpret_cast<T*>(data);

    try
    {
        for (dataPos = 0; dataPos < count; ++dataPos)
        {
            dataTyped[dataPos] = value;
        }
    }
    catch (ExceptionWithReason& exception)
    {
        dtorElements(dataTyped, 0, dataPos - 1);
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while copying vector",
            std::move(exception)
        );
    }
    catch (...)
    {
        dtorElements(dataTyped, 0, dataPos - 1);
        throw;
    }
}

template<typename T, Allocator AllocatorType>
void rewriteData(
    char* data, 
    typename Vector<T, AllocatorType>::ConstIterator first,
    typename Vector<T, AllocatorType>::ConstIterator last
)
{
    T* dataTyped = reinterpret_cast<T*>(data);
    size_t dataPos = 0;
    try
    {
        for (typename Vector<T, AllocatorType>::ConstIterator it = first; it != last; ++it)
        {
            dataTyped[dataPos] = *it;
            dataPos++;
        }
    }
    catch (ExceptionWithReason& exception)
    {
        dtorElements(dataTyped, 0, dataPos - 1);
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorCtorErr,
            "Failed to copy elements while copying vector",
            std::move(exception)
        );
    }
    catch (...)
    {
        dtorElements(dataTyped, 0, dataPos - 1);
        throw;
    }
}

} // namespace anon

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector() noexcept :  data_(nullptr), size_(0), capacity_(0) {}

template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector(size_t size, const T& value) : size_(size), capacity_(size)
{
    data_ = allocateMemoryInBytes<AllocatorType>(capacity_ * sizeof(T));
    
    tryCopyToEmptyDataElseDelete<T, AllocatorType>(data_, size_, value);
}


template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector(
    const ConstIterator& first, const ConstIterator& last
) : size_(last - first), capacity_(size_)
{
    data_ = allocateMemoryInBytes<AllocatorType>(capacity_ * sizeof(T));
    tryCopyToEmptyDataElseDelete<T, AllocatorType>(data_, first, last);
}


template<typename T, Allocator AllocatorType>
Vector<T, AllocatorType>::Vector(const Vector& other) : size_(other.size_), capacity_(other.size_)
{
    data_ = allocateMemoryInBytes<AllocatorType>(size_ * sizeof(T));

    tryCopyToEmptyDataElseDelete<T, AllocatorType>(data_, other.begin(), other.end());
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
    if (data_ == nullptr)
        return;
    
    T* dataTyped = reinterpret_cast<T*>(data_);
    dtorElements(dataTyped, 0, size_);

    delete[] data_;
}

#if 0
template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::assign(size_t count, const T& value)
{
    if (count > capacity_)
        reallocMemory(count);

    size_t kDataToRewrite = std::min(count, size_);
    size_t kDataToFill    = count - kDataToRewrite;

    rewriteData<T, AllocatorType>(data_, kDataToRewrite, value);
    copyToEmptyData<T, AllocatorType>(data_ + kDataToRewrite * sizeof(T), kDataToFill, value);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::assign(Iterator first, Iterator last)
{
    size_t count = last - first;

    if (count > capacity_)
        reallocMemory(count);

    size_t kDataToRewrite = std::min(count, size_);

    Iterator rewriteLast = first + kDataToRewrite; 
    rewriteData<T, AllocatorType>(data_, first, rewriteLast);
    copyToEmptyData<T, AllocatorType>(data_ + kDataToRewrite * sizeof(T), rewriteLast, last);
}
#endif

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
    return reinterpret_cast<const T*>(data_)[pos];
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
    return reinterpret_cast<T*>(data)[0];
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
    return reinterpret_cast<T*>(data)[size_ - 1];
}

template<typename T, Allocator AllocatorType>
T* Vector<T, AllocatorType>::data() noexcept
{
    return const_cast<T*>(const_cast<const Vector<T, AllocatorType>*>(this)->data());
}

template<typename T, Allocator AllocatorType>
const T* Vector<T, AllocatorType>::data() const noexcept
{
    return reinterpret_cast<T*>(data_);
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator Vector<T, AllocatorType>::begin() noexcept
{
    return Iterator{reinterpret_cast<T*>(data_)};
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::Iterator Vector<T, AllocatorType>::end() noexcept
{
    return Iterator{reinterpret_cast<T*>(data_) + size_};
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::ConstIterator Vector<T, AllocatorType>::begin() const noexcept
{
    return ConstIterator{reinterpret_cast<T*>(data_)};
}

template<typename T, Allocator AllocatorType>
typename Vector<T, AllocatorType>::ConstIterator Vector<T, AllocatorType>::end() const noexcept
{
    return ConstIterator{reinterpret_cast<const T*>(data_) + size_};
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
        reallocMemory(newCapacity);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::shrinkToFit()
{
    reallocMemory(size_);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::clear() noexcept
{
    dtorElements(reinterpret_cast<T*>(data_), 0, size_);

    size_ = 0;
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::pushBack(const T& value)
{
    PushResult pushResult = tryPush(value);

    if (pushResult == PushResult::Ok)
        return;

    assert(pushResult == PushResult::NeedToResize);

    Vector<T, AllocatorType> newVector;
    newVector.reserve(getCapacityAfterGrowth(capacity_));

    copyToEmptyData<T, AllocatorType>(newVector.data_, begin(), end());
    newVector.size_ = size_;

    pushResult = newVector.tryPush(value);
    assert(pushResult == PushResult::Ok);

    std::swap(*this, newVector);
}

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::popBack() noexcept
{
    dtorElements(reinterpret_cast<T*>(data_) + size_ - 1, 0, 1);
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
    char* newData = allocateMemoryInBytes<AllocatorType>(newSize * sizeof(T));

    ConstIterator endCopyData = begin() + std::min(size_, newSize);
    copyToEmptyData<T, AllocatorType>(newData, begin(), endCopyData);
    
    if (newSize > size_)
        copyToEmptyData<T, AllocatorType>(newData + size_ * sizeof(T), newSize - size_, value);

    dtorElements(data_, 0, size_);
    delete [] data_;

    data_     = newData;
    size_     = newSize;
    capacity_ = newSize;
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
        new(data_ + size_ * sizeof(T)) T(value);
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

template<typename T, Allocator AllocatorType>
void Vector<T, AllocatorType>::reallocMemory(size_t newCapacity)
{
    assert(newCapacity >= capacity_);

    char* newData = allocateMemoryInBytes<AllocatorType>(newCapacity * sizeof(T));

    copyToEmptyData<T, AllocatorType>(newData, begin(), end());

    dtorElements(reinterpret_cast<T*>(data_), 0, size_);
    delete [] data_;

    data_ = newData;
    /* size_ = size_ */
    capacity_ = newCapacity;
}


} // namespace MyStd

#endif // VECTOR_IMPL_HPP