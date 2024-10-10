#ifndef COMMON_VECTOR_FUNCS_HPP
#define COMMON_VECTOR_FUNCS_HPP

#include <cstddef>
#include "VectorClass.hpp"
#include "Errors.hpp"
#include "Exceptions.hpp"

namespace MyStd
{

inline size_t getCapacityAfterGrowth(size_t capacity) noexcept
{
    static const size_t growthFactor = 2;
    static const size_t minCapacity  = 1; 
    return capacity * growthFactor + minCapacity;
}

} // namespace MyStd

#endif // COMMON_VECTOR_FUNCS_HPP