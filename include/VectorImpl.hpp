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

#define CATCH_EXCEPTION(ERROR, REASON)  \
    catch (ExceptionWithReason& exception) \
    { \
        allocator.dtorElements(from, dataPos - 1); \
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(ERROR, REASON, std::move(exception)); \
    } \
    catch (...) \
    { \
        allocator.dtorElements(from, dataPos - 1); \
        throw; \
    }


template<typename Allocator, typename ConstIterator>
void copyToEmptyData(Allocator& allocator, const size_t from, ConstIterator first, ConstIterator last)
{
    size_t dataPos = from;
    try
    {
        for (ConstIterator it = first; it != last; ++it)
        {
            allocator[dataPos] = *it;
            dataPos++;
        }
    }
    CATCH_EXCEPTION(StdErrors::VectorCtorErr, "Failed to copy elements while copying vector");
}

template<typename T, typename Allocator>
void copyToEmptyData(Allocator& allocator, const size_t from, size_t count, const T& value)
{
    size_t dataPos = from;
    try
    {
        for (dataPos = from; dataPos < count + from; ++dataPos)
        {
            allocator[dataPos] = value;
        }
    }
    CATCH_EXCEPTION(StdErrors::VectorCtorErr, "Failed to copy elements while copying vector");
}

template<typename T, typename Allocator>
void tryCopyToEmptyDataElseDelete(Allocator& allocator, size_t from, size_t count, const T& value)
{
    try
    {
        copyToEmptyData(allocator, from, count, value);
    }
    catch (...)
    {
        allocator.free();
        throw;
    }
}

template<typename T,  typename Allocator, typename ConstIterator>
void tryCopyToEmptyDataElseDelete(
    Allocator& allocator, const size_t from, ConstIterator first, ConstIterator last
)
{
    try
    {
        copyToEmptyData(allocator, from, first, last);
    }
    catch(...)
    {
        allocator.free();
        throw;
    }
}

template<typename T, typename Allocator>
void rewriteData(Allocator& allocator, size_t from, size_t count, const T& value)
{
    size_t dataPos = from;

    try
    {
        for (dataPos = from; dataPos < count + from; ++dataPos)
        {
            allocator[dataPos] = value;
        }
    }
    CATCH_EXCEPTION(StdErrors::VectorCtorErr, "Failed to copy elements while copying vector");
}

template<typename T, typename Allocator, typename ConstIterator>
void rewriteData(Allocator& allocator, size_t from, ConstIterator first, ConstIterator last)
{
    size_t dataPos = from;

    try
    {
        for (ConstIterator it = first; it != last; ++it)
        {
            allocator[dataPos] = *it;
            dataPos++;
        }
    }
    CATCH_EXCEPTION(StdErrors::VectorCtorErr, "Failed to copy elements while copying vector");
}

#undef CATCH_EXCEPTION

} // namespace anon

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_t size, const T& value) : allocator_{size, value}
{
    // std::cout << (1)
    for (size_t i = 0; i < size; ++i)
    {
        // std::cout << (1)
    }
    // std::cout << (1)
}

template<typename T, typename Allocator>
Vector<T, Allocator>::Vector(const ConstIterator& first, const ConstIterator& last) : allocator_{last - first}
{
    tryCopyToEmptyDataElseDelete(allocator_, 0, first, last);
}

template<typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& other)
{
    Vector<T, Allocator> copy(other);
    swap(copy);
    return *this;
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::at(size_t pos)
{
    if (pos >= allocator_.size())
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorIndexOutOfBounds,
            "Vector index out of bounds",
            {}
        );
    }

    return allocator_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator::ConstReference Vector<T, Allocator>::at(size_t pos) const
{
    if (pos >= allocator_.size())
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::VectorIndexOutOfBounds,
            "Vector index out of bounds",
            {}
        );
    }

    return allocator_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::operator[](size_t pos) noexcept
{
    return allocator_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator::ConstReference Vector<T, Allocator>::operator[](size_t pos) 
    const noexcept
{
    return allocator_[pos];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::front() noexcept
{
    return allocator_[0];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator::ConstReference Vector<T, Allocator>::front() const noexcept
{  
    return allocator_[0];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator::Reference Vector<T, Allocator>::back() noexcept
{
    return allocator_[allocator_.size() - 1];
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator::ConstReference Vector<T, Allocator>::back() const noexcept
{
    return allocator_[allocator_.size() - 1];
}

template<typename T, typename Allocator>
T* Vector<T, Allocator>::data() noexcept
{
    return allocator_.data();
}

template<typename T, typename Allocator>
const T* Vector<T, Allocator>::data() const noexcept
{
    return allocator_.data();
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator Vector<T, Allocator>::begin() noexcept
{
    return Iterator{allocator_[0]};
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::Iterator Vector<T, Allocator>::end() noexcept
{
    return Iterator{allocator_[allocator_.size()]};
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator Vector<T, Allocator>::begin() const noexcept
{
    return ConstIterator{allocator_.data()};
}

template<typename T, typename Allocator>
typename Vector<T, Allocator>::ConstIterator Vector<T, Allocator>::end() const noexcept
{
    return ConstIterator{allocator_.data() + allocator_.size()};
}

template<typename T, typename Allocator>
bool Vector<T, Allocator>::empty() const noexcept
{
    return allocator_.size() == 0;
}

template<typename T, typename Allocator>
size_t Vector<T, Allocator>::size() const noexcept
{
    return allocator_.size();
}

template<typename T, typename Allocator>
size_t Vector<T, Allocator>::capacity() const noexcept
{
    return allocator_.capacity();
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::reserve(size_t newCapacity)
{
    if (newCapacity > allocator_.capacity())
        allocator_.realloc(newCapacity);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::shrinkToFit()
{
    allocator_.realloc(allocator_.size());
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::clear() noexcept
{
    allocator_.dtorElements(0, allocator_.size());
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::pushBack(const T& value)
{
    PushResult pushResult = tryPush(value);

    if (pushResult == PushResult::Ok)
        return;

    assert(pushResult == PushResult::NeedToResize);

    Vector<T, Allocator> newVector;

    newVector.reserve(getCapacityAfterGrowth(allocator_.capacity()));
    
    copyToEmptyData(newVector.allocator_, 0, begin(), end());

    pushResult = newVector.tryPush(value);
    assert(pushResult == PushResult::Ok);

    swap(newVector);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::popBack() noexcept
{
    allocator_.dtorElements(allocator_.size() - 1, allocator_.size());
}

// allocator - allocateInBytes, realloc, freeMemory, resizeData, data(), size(), capacity()

template<typename T, typename Allocator>
void Vector<T, Allocator>::resize(size_t newSize, const T& value)
{
    Allocator newAllocator{newSize, value};

    ConstIterator endCopyData = begin() + std::min(allocator_.size(), newSize);
    copyToEmptyData(newAllocator, 0, begin(), endCopyData);

    allocator_.swap(newAllocator);
}

template<typename T, typename Allocator>
void Vector<T, Allocator>::swap(Vector& other)
{
    allocator_.swap(other.allocator_);
}

// ------------------------------Private------------------------------

template<typename T, typename Allocator>
typename Vector<T, Allocator>::PushResult Vector<T, Allocator>::tryPush(const T& value)
{   
    if (allocator_.size() >= allocator_.capacity())
        return PushResult::NeedToResize;

    try
    {
        allocator_[allocator_.size()] = value;
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

#endif // VECTOR_IMPL_HPP