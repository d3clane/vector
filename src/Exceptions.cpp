#include "Exceptions.hpp"

namespace MyStd
{

ExceptionWithReason::ExceptionWithReason() noexcept
{
}

ExceptionWithReason::ExceptionWithReason(
    StdErrors error, const char* reason, ExceptionWithReason&& prevException
) noexcept
{
    ExceptionWithReason* storageForPrevException = nullptr;

    try
    {
        storageForPrevException = new ExceptionWithReason(std::move(prevException));
    }
    catch (std::bad_alloc)
    {
        fprintf(stderr, "CAUGHT BAD ALLOC WHILE CREATING NEXT EXCEPTION\n. Terminating.\n");
        fprintf(stderr, "%s", prevException.what());

        std::terminate();
    }
    catch (...)
    {
        fprintf(stderr, "UNKNOWN EXCEPTION CAUGHT WHILE CREATING NEXT EXCEPTION\n. Terminating.\n");
        fprintf(stderr, "%s", prevException.what());

        std::terminate();
    }

    assert(storageForPrevException);

    prevException_.reset(storageForPrevException);
    reasonString_.reset(strdup(reason));
    error_ = error;
}

ExceptionWithReason::ExceptionWithReason(
    StdErrors error, const char* reason, ExceptionWithReason&& prevException,
    const char* funcWithErr, const char* fileWithErr, const size_t lineWithErr
) noexcept
{
    static const size_t bufferForReasonSize = 1024;
    static char extendedReason[bufferForReasonSize] = "";

    snprintf(
        extendedReason, bufferForReasonSize, "%s Occurred in func %s, file %s, line %zu", 
        reason, funcWithErr, fileWithErr, lineWithErr
    );
    
    *this = ExceptionWithReason{error, extendedReason, std::move(prevException)};
}

ExceptionWithReason::ExceptionWithReason(ExceptionWithReason&& other) noexcept
{
    error_ = other.error_;
    reasonString_.reset(other.reasonString_.release());
    prevException_.reset(other.prevException_.release());
}

ExceptionWithReason& ExceptionWithReason::operator=(ExceptionWithReason&& other) noexcept
{
    error_ = other.error_;
    reasonString_.reset(other.reasonString_.release());
    prevException_.reset(other.prevException_.release());

    return *this;
}

const char* ExceptionWithReason::what() const noexcept
{
    const ExceptionWithReason* exceptionNow = this;

    fprintf(stderr, "Exception occurred. Reasons:\n");

    size_t reasonId = 0;
    const char* reason = nullptr;

    while (exceptionNow)
    {
        reason = exceptionNow->reasonString_.get();

        if (reason) 
        {
            fprintf(
                stderr, "%zu. %s Error code - %d\n", reasonId, reason, static_cast<int>(exceptionNow->error_)
            );
            
            reasonId++;
        }

        exceptionNow = exceptionNow->prevException_.get();
    }

    return "";
}

} // namespace MyStd
