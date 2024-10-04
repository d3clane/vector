#ifndef VECTOR_ITERATOR_IMPL_HPP
#define VECTOR_ITERATOR_IMPL_HPP

#include "VectorIteratorClass.hpp"

namespace MyStd
{

template<typename T>
VectorIterator<T>::VectorIterator(T* ptr) noexcept : ptr_(ptr) {}

template<typename T>
VectorIterator<T>& VectorIterator<T>::operator++() noexcept
{
    ++ptr_;
    return *this;
}

template<typename T>
VectorIterator<T> VectorIterator<T>::operator++(int) noexcept
{
    VectorIterator<T> tmp = *this;
    ++(*this);
    return tmp;
}

template<typename T>
VectorIterator<T>& VectorIterator<T>::operator--() noexcept
{
    --ptr_;
    return *this;
}

template<typename T>
VectorIterator<T> VectorIterator<T>::operator--(int) noexcept
{
    VectorIterator<T> tmp = *this;
    --(*this);
    return tmp;
}

template<typename T>
VectorIterator<T>& VectorIterator<T>::operator+=(Difference delta) noexcept
{
    ptr_ += delta;
    return *this;
}

template<typename T>
VectorIterator<T>& VectorIterator<T>::operator-=(Difference delta) noexcept
{
    ptr_ -= delta;
    return *this;
}

template<typename T>
typename VectorIterator<T>::Reference VectorIterator<T>::operator*() const
{
    return *ptr_;
}

template<typename T>
typename VectorIterator<T>::Pointer VectorIterator<T>::operator->() const
{
    return ptr_;
}

template<typename T>
VectorIterator<T> operator+(const VectorIterator<T>& lhs, typename VectorIterator<T>::Difference delta) noexcept
{
    VectorIterator<T> tmp = *lhs;
    tmp += delta;
    return tmp;
}

template<typename T>
VectorIterator<T> operator-(const VectorIterator<T>& lhs, typename VectorIterator<T>::Difference delta) noexcept
{
    VectorIterator<T> tmp = *lhs;
    tmp -= delta;
    return tmp;
}

template<typename T>
bool operator==(const VectorIterator<T>& lhs, const VectorIterator<T>& rhs) noexcept
{
    return lhs.ptr_ == rhs.ptr_;
}

template<typename T>
bool operator!=(const VectorIterator<T>& lhs, const VectorIterator<T>& rhs) noexcept
{
    return !(lhs == rhs);
}

template<typename T>
bool operator<(const VectorIterator<T>& lhs, const VectorIterator<T>& rhs) noexcept
{
    return lhs.ptr_ < rhs.ptr_;
}

template<typename T>
bool operator<=(const VectorIterator<T>& lhs, const VectorIterator<T>& rhs) noexcept
{
    return !(lhs > rhs);
}

template<typename T>
bool operator>(const VectorIterator<T>& lhs, const VectorIterator<T>& rhs) noexcept
{
    return rhs < lhs;
}

template<typename T>
bool operator>=(const VectorIterator<T>& lhs, const VectorIterator<T>& rhs) noexcept
{
    return !(lhs < rhs);
}

} // namespace myStd

#endif // VECTOR_ITERATOR_IMPL_HPP