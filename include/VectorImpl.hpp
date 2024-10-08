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

template<typename T, typename ConstIterator>
void copyToEmptyData(char* data, ConstIterator first, ConstIterator last)
{
    size_t dataPos = 0;
    try
    {
        for (ConstIterator it = first; it != last; ++it)
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

template<typename T>
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

template<typename T, typename Allocator>
void tryCopyToEmptyDataElseDelete(char* data, size_t count, const T& value, Allocator& allocator_)
{
    try
    {
        copyToEmptyData(data, count, value);
    }
    catch (...)
    {
        allocator_.free(data);
        throw;
    }
}

template<typename T, typename ConstIterator, typename Allocator>
void tryCopyToEmptyDataElseDelete(char* data, ConstIterator first, ConstIterator last, Allocator& allocator_)
{
    try
    {
        copyToEmptyData(data, first, last);
    }
    catch(...)
    {
        allocator_.free(data);
        throw;
    }
}

template<typename T>
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

template<typename T, typename ConstIterator>
void rewriteData(char* data, ConstIterator first, ConstIterator last)
{
    T* dataTyped = reinterpret_cast<T*>(data);
    size_t dataPos = 0;
    try
    {
        for (ConstIterator it = first; it != last; ++it)
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

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_t size, const T& value) : allocator_(Allocator(size))
{
    data_ = allocator_.allocateMemoryInBytes(capacity_ * sizeof(T));
    
    tryCopyToEmptyDataElseDelete(data_, size_, value);
}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const ConstIterator& first, const ConstIterator& last) 
    : size_(last - first), capacity_(size_)
{
    data_ = allocator_.allocateMemoryInBytes(capacity_ * sizeof(T));

    tryCopyToEmptyDataElseDelete(data_, first, last);
}


template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector& other) : size_(other.size_), capacity_(other.size_)
{
    data_ = allocator_.allocateMemoryInBytes(capacity_ * sizeof(T));

    tryCopyToEmptyDataElseDelete(data_, other.begin(), other.end());
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(Vector&& other) noexcept : size_(other.size_), capacity_(other.size_)
{
    data_ = other.data_;
    other.data_ = nullptr;
}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& other)
{
    Vector<T, Allocator> copy(other);
    std::swap(*this, copy);
    return *this;
}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& other) noexcept
{
    size_       = other.size_;
    capacity_   = other.capacity_;
    data_       = other.data_;
    other.data_ = nullptr;
    return *this;
}

template<typename T, typename Allocator>
Vector<T, Allocator>::~Vector()
{
    if (data_ == nullptr)
        return;
    
    T* dataTyped = reinterpret_cast<T*>(data_);
    dtorElements(dataTyped, 0, size_);

    delete[] data_;
}

#if 0
template<typename T, typename Allocator>
void Vector<T, Allocator>::assign(size_t count, const T& value)
{
    if (count > capacity_)
    {
        reallocMemory(count);
        capacity_ = count;
    }

    size_t kDataToRewrite = std::min(count, size_);
    size_t kDataToFill    = count - kDataToRewrite;

    rewriteData<T, Allocator>(data_, kDataToRewrite, value);
    copyToEmptyData<T, Allocator>(data_ + kDataToRewrite * sizeof(T), kDataToFill, value);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::assign(Iterator first, Iterator last)
{
    size_t count = last - first;

    if (count > capacity_)
    {
        reallocMemory(count);
        capacity_ = count;
    }

    size_t kDataToRewrite = std::min(count, size_);

    Iterator rewriteLast = first + kDataToRewrite; 
    rewriteData<T, Allocator>(data_, first, rewriteLast);
    copyToEmptyData<T, Allocator>(data_ + kDataToRewrite * sizeof(T), rewriteLast, last);
}
#endif

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::at(size_t pos)
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, Allocator>*>(this)->at(pos)
    );
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::ConstReference Vector<T, Allocator>::at(size_t pos) const
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

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::operator[](size_t pos) noexcept
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, Allocator>*>(this)->operator[](pos)
    );
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::ConstReference Vector<T, Allocator>::operator[](
    size_t pos
) const noexcept
{
    return reinterpret_cast<const T*>(data_)[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::front() noexcept
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, Allocator>*>(this)->front()
    );
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::ConstReference Vector<T, Allocator>::front() const noexcept
{
    return reinterpret_cast<T*>(data)[0];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::back() noexcept
{
    return const_cast<typename Iterator::Reference>(
        const_cast<const Vector<T, Allocator>*>(this)->back()
    );
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::ConstReference Vector<T, Allocator>::back() const noexcept
{
    return reinterpret_cast<T*>(data)[size_ - 1];
}

template<typename T, typename Allocator>
T* Vector<T, Allocator>::data() noexcept
{
    return const_cast<T*>(const_cast<const Vector<T, Allocator>*>(this)->data());
}

template<typename T, typename Allocator>
const T* Vector<T, Allocator>::data() const noexcept
{
    return reinterpret_cast<T*>(data_);
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator Vector<T, Allocator>::begin() noexcept
{
    return Iterator{reinterpret_cast<T*>(data_)};
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator Vector<T, Allocator>::end() noexcept
{
    return Iterator{reinterpret_cast<T*>(data_) + size_};
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator Vector<T, Allocator>::begin() const noexcept
{
    return ConstIterator{reinterpret_cast<T*>(data_)};
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator Vector<T, Allocator>::end() const noexcept
{
    return ConstIterator{reinterpret_cast<const T*>(data_) + size_};
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::empty() const noexcept
{
    return size_ == 0;
}

template<typename T, typename Allocator>
size_t Vector<T, Allocator>::size() const noexcept
{
    return size_;
}

template<typename T, typename Allocator>
size_t Vector<T, Allocator>::capacity() const noexcept
{
    return capacity_;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::reserve(size_t newCapacity)
{
    if (newCapacity > capacity_)
    {
        data_ = allocator_.reallocMemory(data_, capacity_, newCapacity);
        capacity_ = newCapacity;
    }
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::shrinkToFit()
{
    data_ = allocator_.reallocMemory(data_, capacity_, size_);
    capacity_ = size_;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::clear() noexcept
{
    dtorElements(reinterpret_cast<T*>(data_), 0, size_);

    size_ = 0;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::pushBack(const T& value)
{
    PushResult pushResult = tryPush(value);

    if (pushResult == PushResult::Ok)
        return;

    assert(pushResult == PushResult::NeedToResize);

    Vector<T, Allocator> newVector;
    newVector.reserve(getCapacityAfterGrowth(capacity_));

    copyToEmptyData<T, Allocator>(newVector.data_, begin(), end());
    newVector.size_ = size_;

    pushResult = newVector.tryPush(value);
    assert(pushResult == PushResult::Ok);

    std::swap(*this, newVector);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::popBack() noexcept
{
    dtorElements(reinterpret_cast<T*>(data_) + size_ - 1, 0, 1);
    size_--;
}

#if 0
template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator Vector<T, Allocator>::insert(Iterator pos, const T& value)
{
    
}

Iterator insert(Iterator pos, size_t count, const T& value);
Iterator insert(Iterator pos, Iterator first, Iterator last);

Iterator erase(Iterator pos);
Iterator erase(Iterator first, Iterator last);
#endif

// allocator - allocateMemoryInBytes, reallocMemory, freeMemory, resizeData, data(), size(), capacity()

template<typename T, typename Allocator>
void Vector<T, Allocator>::resize(size_t newSize, const T& value)
{
    allocator_.resize(data_, )
    char* newData = allocator_.allocateMemoryInBytes(newSize * sizeof(T));

    ConstIterator endCopyData = begin() + std::min(size_, newSize);
    copyToEmptyData(newData, begin(), endCopyData);
    
    if (newSize > size_)
        copyToEmptyData(newData + size_ * sizeof(T), newSize - size_, value);

    dtorElements(data_, 0, size_);
    allocator_.free(data_);

    data_     = newData;
    size_     = newSize;
    capacity_ = newSize;
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::swap(Vector& other)
{
    std::swap(data_, other.data_); // using my move ctor
}

// ------------------------------Private------------------------------

template<typename T, typename Allocator>
typename Vector<T, Allocator>::PushResult Vector<T, Allocator>::tryPush(const T& value)
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

template<typename T, typename Allocator>
void Vector<T, Allocator>::reallocMemory(size_t newCapacity)
{
    char* newData = allocator_.allocateMemoryInBytes(newCapacity * sizeof(T));

    ConstIterator endCopyData = begin() + std::min(size_, newCapacity);
    copyToEmptyData(newData, begin(), endCopyData);

    dtorElements(reinterpret_cast<T*>(data_), 0, size_);
    allocator_.free(data_);
    //delete [] data_;

    data_ = newData;
}


} // namespace MyStd

#endif // VECTOR_IMPL_HPP