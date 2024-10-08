#ifndef BOOL_VECTOR_HPP
#define BOOL_VECTOR_HPP

#include <cstddef>
#include <cstdint>

#include "VectorClass.hpp"

namespace MyStd
{

template<Allocator AllocatorType>
class Vector<bool, AllocatorType> final
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
    Vector() noexcept;

    explicit Vector(size_t size, const bool value = false);
    Vector(const Vector& other);
    Vector(Vector&& other) noexcept;

    Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other) noexcept;

    ~Vector();

    bool at(size_t pos);
    bool at(size_t pos) const;

    ProxyValue operator[](size_t pos) noexcept;
    bool operator[](size_t pos) const noexcept;

    bool front() noexcept;
    bool front() const noexcept;

    bool back() noexcept;
    bool back() const noexcept;

    bool*       data() noexcept;
    const bool* data() const noexcept;

    bool   empty   () const noexcept;
    size_t size    () const noexcept;
    size_t capacity() const noexcept;

    void reserve(size_t newCapacity);

    void shrinkToFit();
    void clear() noexcept;

    void pushBack(const bool value);
    void popBack() noexcept;

    void resize(size_t newSize, const bool value = false);

    void swap(Vector& other);

private:
    enum class PushResult
    {
        Ok,
        NeedToResize
    };

    PushResult tryPush(const bool value);

    void reallocMemory(size_t newCapacity);
};

} // namespace MyStd

#endif // BOOL_VECTOR_HPP
