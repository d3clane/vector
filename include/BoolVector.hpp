#ifndef BOOL_VECTOR_HPP
#define BOOL_VECTOR_HPP

#include <cstddef>
#include <cstdint>

#include "VectorClass.hpp"

namespace MyStd
{

template<Allocator AllocatorType>
class Vector<bool, AllocatorType>
{
    struct ProxyValue
    {
        uint8_t* data_;
        uint8_t mask_;

        ProxyValue() noexcept : data_(nullptr), mask_(0) {}
        ProxyValue(uint8_t* data, uint8_t mask) noexcept : data_(data), mask_(mask) {}

        void operator=(const bool value);

        operator bool() const;
    };

    uint8_t* data_;
    size_t size_;
    size_t capacity_;

public:
    using Iterator = VectorIterator<bool>;

    Vector() noexcept;

    explicit Vector(size_t size, const bool value = false); // TODO: noexcept based on functions in Vector
    Vector(Iterator first, Iterator last);
    Vector(Vector& other); // TODO: const Vector& other should be. Like that because I don't have const iterator
    // TODO: create it or think about it more (maybe like stl input iterator smth like that)

    Vector(Vector&& other) noexcept;

    Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other) noexcept;

    ~Vector();

    void assign(size_t count, const bool value);
    void assign(Iterator first, Iterator last);

    typename Iterator::Reference      at(size_t pos);
    typename Iterator::ConstReference at(size_t pos) const;

    ProxyValue operator[](size_t pos) noexcept;

    typename Iterator::Reference      front() noexcept;
    typename Iterator::ConstReference front() const noexcept;

    typename Iterator::Reference      back() noexcept;
    typename Iterator::ConstReference back() const noexcept;

    bool*       data() noexcept;
    const bool* data() const noexcept;

    Iterator begin() noexcept;
    Iterator end  () noexcept;

    const Iterator begin() const noexcept;
    const Iterator end  () const noexcept;

    // TODO: rbegin(), rend()

    bool   empty   () const noexcept;
    size_t size    () const noexcept;
    size_t capacity() const noexcept;

    void reserve(size_t newCapacity);

    void shrinkToFit();
    void clear() noexcept;

    void pushBack(const bool value);
    void popBack() noexcept;

#if 0
    Iterator insert(Iterator pos, const T& value);
    Iterator insert(Iterator pos, size_t count, const T& value);
    Iterator insert(Iterator pos, Iterator first, Iterator last);

    Iterator erase(Iterator pos);
    Iterator erase(Iterator first, Iterator last);
#endif

    void resize(size_t newSize, const bool value = false);

    void swap(Vector& other);

private:
    enum class PushResult
    {
        Ok,
        NeedToResize
    };

    PushResult tryPush(const bool value);
};


// -------------------------------------Implementation---------------------------------


} // namespace MyStd

#endif // BOOL_VECTOR_HPP
