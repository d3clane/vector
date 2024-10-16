#ifndef ERRORS_HPP
#define ERRORS_HPP

namespace MyStd
{

enum class StdErrors
{
    Ok = 0,
    MemAllocErr,
    VectorCtorErr,
    VectorIndexOutOfBounds,
    VectorOnStackNotEnoughMemory,
    AllocatorCtorErr,
};

} // namespace MyStd

#endif // ERRORS_HPP