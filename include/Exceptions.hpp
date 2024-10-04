#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <memory>
#include <iostream>

#include "Errors.hpp"

namespace MyStd
{

#define EXCEPTION_WITH_REASON_CREATE_NEXT_EXCEPTION(ERROR, REASON, PREV_EXCEPTION) \
    ExceptionWithReason{ERROR, REASON, PREV_EXCEPTION, __func__, __FILE__, __LINE__}

class ExceptionWithReason : public std::exception
{
    StdErrors error_;

    std::unique_ptr<char> reasonString_;
    std::unique_ptr<ExceptionWithReason> prevException_;

    std::unique_ptr<std::exception> prevStdException_;

public:
    ExceptionWithReason() noexcept;

    ExceptionWithReason(
        StdErrors error, const char* reason, ExceptionWithReason&& prevException
    ) noexcept;
    ExceptionWithReason(
        StdErrors error, const char* reason, ExceptionWithReason&& prevException,
        const char* funcWithErr, const char* fileWithErr, const size_t lineWithErr
    ) noexcept;

    ExceptionWithReason(const ExceptionWithReason& other) = delete;
    ExceptionWithReason(ExceptionWithReason&& other) noexcept;

    ExceptionWithReason& operator=(const ExceptionWithReason& other) = delete;
    ExceptionWithReason& operator=(ExceptionWithReason&& other) noexcept;

    const char* what() const noexcept override;

    ~ExceptionWithReason() = default;
};

} // namespace MyStd

#endif // EXCEPTIONS_HPP