#include "OpButton/OpButtonLog.h"

#include <cstdio>
#include <cstdarg>

#ifdef _WIN32
#include "OpButtonWin32.h"
#endif

namespace OpButton {

OpButtonLogLevel OpButtonLog::s_level = OpButtonLogLevel::PL_INFO;
OpButtonLoggerListener* OpButtonLog::s_listener = nullptr;
const char* OpButtonLog::TAG = "OpButtonSDK";

void OpButtonLog::setLevel(OpButtonLogLevel level) {
    s_level = level;
}

void OpButtonLog::setLoggerListener(OpButtonLoggerListener* listener) {
    s_listener = listener;
}

void OpButtonLog::logMessage(OpButtonLogLevel level, const char* fmt, va_list args) {
    if (static_cast<int>(level) < static_cast<int>(s_level)) {
        return;
    }

    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, args);

    if (s_listener) {
        s_listener->onOpButtonLoggerMessage(level, TAG, buf);
        return;
    }

    const char* levelStr = opbuttonLogLevelValue(level);

#ifdef _WIN32
    char dbgBuf[1100];
    snprintf(dbgBuf, sizeof(dbgBuf), "[%s] %s: %s\n", levelStr, TAG, buf);
    OutputDebugStringA(dbgBuf);
#endif

    fprintf(stderr, "[%s] %s: %s\n", levelStr, TAG, buf);
}

void OpButtonLog::d(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(OpButtonLogLevel::PL_DEBUG, fmt, args);
    va_end(args);
}

void OpButtonLog::i(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(OpButtonLogLevel::PL_INFO, fmt, args);
    va_end(args);
}

void OpButtonLog::w(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(OpButtonLogLevel::PL_WARNING, fmt, args);
    va_end(args);
}

void OpButtonLog::e(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(OpButtonLogLevel::PL_ERROR, fmt, args);
    va_end(args);
}

} // namespace OpButton
