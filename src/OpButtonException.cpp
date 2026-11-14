#include "OpButton/OpButtonException.h"

namespace OpButton {

OpButtonException::OpButtonException(const OpButtonString& message)
    : m_message(message), m_httpCode(0) {
}

OpButtonException::OpButtonException(int httpCode, const OpButtonString& message)
    : m_message(message), m_httpCode(httpCode) {
}

const char* OpButtonException::what() const noexcept { return m_message.c_str(); }
int OpButtonException::httpCode() const { return m_httpCode; }

bool OpButtonException::isNetworkError() const { return m_httpCode == 0; }
bool OpButtonException::isSessionGone() const { return m_httpCode == 410; }
bool OpButtonException::isUnauthorized() const { return m_httpCode == 401; }

} // namespace OpButton
