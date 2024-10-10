#ifndef BOOL_VECTOR_HPP
#define BOOL_VECTOR_HPP

#include <cstddef>
#include <cstdint>

#include "VectorClass.hpp"
#include "Allocators/DynamicAllocator.hpp"

namespace MyStd
{

template<typename Allocator>
class Vector<bool, Allocator> final
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

    Allocator allocator_;
    size_t size_;

public:
    Vector() noexcept;

    explicit Vector(size_t size);
    explicit Vector(size_t size, const bool value);

    bool at(size_t pos);
    bool at(size_t pos) const;

    ProxyValue operator[](size_t pos) noexcept;
    bool operator[](size_t pos) const noexcept;

    ProxyValue front() noexcept;
    bool front() const noexcept;

    ProxyValue back() noexcept;
    bool back() const noexcept;

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
};

} // namespace MyStd

#endif // BOOL_VECTOR_HPP
