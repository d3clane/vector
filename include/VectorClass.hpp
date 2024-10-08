#ifndef VECTOR_CLASS_HPP
#define VECTOR_CLASS_HPP

#include <stddef.h>

#include "VectorIteratorClass.hpp"

namespace MyStd
{

template <typename T, typename Allocator>
class Vector final
{
    Allocator allocator_; // allocator can alloc memory, realloc memory, free memory. Also can store static mem

public:
    using Iterator      = VectorIterator<T>;
    using ConstIterator = VectorIterator<const T>;

    Vector() = default;
    explicit Vector(size_t size, const T& value = T()); 
    Vector(const ConstIterator& first, const ConstIterator& last);
    Vector(const Vector& other);

    Vector(Vector&& other) noexcept;

    Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other) noexcept;

    ~Vector();

#if 0
    void assign(size_t count, const T& value);
    void assign(const Iterator first, Iterator last);
#endif

    typename Iterator::Reference      at(size_t pos);
    typename Iterator::ConstReference at(size_t pos) const;

    typename Iterator::Reference      operator[](size_t pos) noexcept;
    typename Iterator::ConstReference operator[](size_t pos) const noexcept;

    typename Iterator::Reference      front() noexcept;
    typename Iterator::ConstReference front() const noexcept;

    typename Iterator::Reference      back() noexcept;
    typename Iterator::ConstReference back() const noexcept;

    T*       data() noexcept;
    const T* data() const noexcept;

    Iterator begin() noexcept;
    Iterator end  () noexcept;

    ConstIterator begin() const noexcept;
    ConstIterator end  () const noexcept;

    bool   empty   () const noexcept;
    size_t size    () const noexcept;
    size_t capacity() const noexcept;

    void reserve(size_t newCapacity);

    void shrinkToFit();
    void clear() noexcept;

    void pushBack(const T& value);
    void popBack() noexcept;

#if 0
    Iterator insert(Iterator pos, const T& value);
    Iterator insert(Iterator pos, size_t count, const T& value);
    Iterator insert(Iterator pos, Iterator first, Iterator last);

    Iterator erase(Iterator pos);
    Iterator erase(Iterator first, Iterator last);
#endif

    void resize(size_t newSize, const T& value = T());

    void swap(Vector& other);

private:
    enum class PushResult
    {
        Ok,
        NeedToResize,
    };

    PushResult tryPush(const T& value);

    void reallocMemory(size_t newSize);
};

#if 0
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
#endif

} // namespace myStd

#endif // VECTOR_CLASS_HPP