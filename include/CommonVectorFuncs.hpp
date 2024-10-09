#ifndef COMMON_VECTOR_FUNCS_HPP
#define COMMON_VECTOR_FUNCS_HPP

#include <cstddef>
#include "VectorClass.hpp"
#include "Errors.hpp"
#include "Exceptions.hpp"

namespace MyStd
{

char* allocateMemoryInBytes(size_t sizeInBytes)
{
    char* data = nullptr;

    try
    {
        data = new char[sizeInBytes];
    }
    catch (const std::bad_alloc& e)
    {
        throw EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(
            StdErrors::MemAllocErr,
            "Failed to allocate memory for vector",
            {}
        );
    }

    return data;
}

template<typename T>
void dtorElements(T* data, size_t from, size_t to) noexcept
{
    for (size_t i = from; i < to; ++i)
    {
        data[i].~T();
    }
}

inline size_t getCapacityAfterGrowth(size_t capacity) noexcept
{
    static const size_t growthFactor = 2;
    static const size_t minCapacity  = 1; 
    return capacity * growthFactor + minCapacity;
}

} // namespace MyStd

#endif // COMMON_VECTOR_FUNCS_HPP