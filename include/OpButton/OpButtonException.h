#pragma once

#include "OpButton/OpButtonTypes.h"
#include <exception>

namespace OpButton
{
    class OpButtonException
        : public std::exception
    {
    public:
        OpButtonException( const OpButtonString & _message );
        OpButtonException( int _httpCode, const OpButtonString & _message );
        ~OpButtonException() override;

    public:
        const char * what() const noexcept override;
        int httpCode() const;

        bool isNetworkError() const;
        bool isSessionGone() const;
        bool isUnauthorized() const;

    private:
        OpButtonString m_message;
        int m_httpCode;
    };

} // namespace OpButton
