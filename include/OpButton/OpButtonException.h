#pragma once

#include "OpButton/OpButtonTypes.h"
#include <exception>

namespace OpButton {

class OpButtonException : public std::exception {
public:
    OpButtonException(const OpButtonString& message);
    OpButtonException(int httpCode, const OpButtonString& message);

    const char* what() const noexcept override;
    int httpCode() const;

    bool isNetworkError() const;
    bool isSessionGone() const;
    bool isUnauthorized() const;

private:
    OpButtonString m_message;
    int m_httpCode;
};

} // namespace OpButton
