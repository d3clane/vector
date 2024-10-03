#ifndef VECTOR_CLASS_HPP
#define VECTOR_CLASS_HPP

#include "VectorIteratorClass.hpp"

namespace myStd
{

enum class Allocator
{
    StaticMem,
    DynamicMem,
};

template <typename T, Allocator AllocatorType = Allocator::StaticMem>
class Vector
{
    T* data_;
    size_t size_;
    size_t capacity_;

public:
    using Iterator = VectorIterator<T>;

    Vector() noexcept;

    explicit Vector(size_t size, const T& value = T()); // TODO: noexcept based on functions in Vector
    Vector(Iterator first, Iterator last);
    Vector(const Vector& other);

    Vector& operator=(const Vector& other);

    ~Vector();

    void assign(size_t count, const T& value);
    void assign(Iterator first, Iterator last);

    Iterator::Reference      at(size_t pos);
    Iterator::ConstReference at(size_t pos) const;

    Iterator::Reference      operator[](size_t pos);
    Iterator::ConstReference operator[](size_t pos) const;

    Iterator::Reference      front();
    Iterator::ConstReference front() const;

    Iterator::Reference      back();
    Iterator::ConstReference back() const;

    T*       data();
    const T* data() const;

    Iterator begin();
    Iterator end();

    // TODO: rbegin(), rend()

    bool empty() const;
    size_t size() const;
    size_t capacity() const;
    void reserve(size_t newCapacity);

    void shrinkToFit();
    void clear();

    void push_back(const T& value);
    void pop_back();

    Iterator insert(Iterator pos, const T& value);
    Iterator insert(Iterator pos, size_t count, const T& value);
    Iterator insert(Iterator pos, Iterator first, Iterator last);

    Iterator erase(Iterator pos);
    Iterator erase(Iterator first, Iterator last);

    void resize(size_t newSize, const T& value = T());

    void swap(Vector& other);
};

template<typename T, Allocator AllocatorType>
bool operator==(const Vector<T, AllocatorType>& lhs, const Vector<T, AllocatorType>& rhs);

template<typename T, Allocator AllocatorType>
bool operator!=(const Vector<T, AllocatorType>& lhs, const Vector<T, AllocatorType>& rhs);

template<typename T, Allocator AllocatorType>
bool operator<(const Vector<T, AllocatorType>& lhs, const Vector<T, AllocatorType>& rhs);

template<typename T, Allocator AllocatorType>
bool operator<=(const Vector<T, AllocatorType>& lhs, const Vector<T, AllocatorType>& rhs);

template<typename T, Allocator AllocatorType>
bool operator>(const Vector<T, AllocatorType>& lhs, const Vector<T, AllocatorType>& rhs);

template<typename T, Allocator AllocatorType>
bool operator>=(const Vector<T, AllocatorType>& lhs, const Vector<T, AllocatorType>& rhs);

} // namespace myStd

#endif // VECTOR_CLASS_HPP