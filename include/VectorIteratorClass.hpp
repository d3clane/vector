#ifndef VECTOR_ITERATOR_CLASS_HPP
#define VECTOR_ITERATOR_CLASS_HPP

namespace myStd
{

template <typename T>
class VectorIterator
{
    T* ptr_;

public:
    using ValueType      = T;
    using Pointer        = T*;
    using ConstPointer   = const T*;
    using Reference      = T&;
    using ConstReference = const T&;
    using Difference     = std::ptrdiff_t;

    explicit VectorIterator(T* ptr = nullptr) noexcept;
    
    VectorIterator& operator++()    noexcept;
    VectorIterator  operator++(int) noexcept;

    VectorIterator& operator--()    noexcept;
    VectorIterator  operator--(int) noexcept;

    VectorIterator& operator+=(Difference delta) noexcept;
    VectorIterator& operator-=(Difference delta) noexcept;

    Reference operator* () const;
    Pointer   operator->() const;

    template<typename U>
    friend bool operator==(const VectorIterator<U>& lhs, const VectorIterator<U>& rhs) noexcept;

    template<typename U>
    friend bool operator!=(const VectorIterator<U>& lhs, const VectorIterator<U>& rhs) noexcept;

    template<typename U>
    friend bool operator< (const VectorIterator<U>& lhs, const VectorIterator<U>& rhs) noexcept;

    template<typename U>
    friend bool operator<=(const VectorIterator<U>& lhs, const VectorIterator<U>& rhs) noexcept;

    template<typename U>
    friend bool operator> (const VectorIterator<U>& lhs, const VectorIterator<U>& rhs) noexcept;
    
    template<typename U>
    friend bool operator>=(const VectorIterator<U>& lhs, const VectorIterator<U>& rhs) noexcept;
};


template <typename T>
VectorIterator<T> operator+(const VectorIterator<T>& lhs, typename VectorIterator<T>::Difference delta) noexcept;

template <typename T>
VectorIterator<T> operator-(const VectorIterator<T>& lhs, typename VectorIterator<T>::Difference delta) noexcept;

} // namespace myStd


#endif // VECTOR_ITERATOR_CLASS_HPP