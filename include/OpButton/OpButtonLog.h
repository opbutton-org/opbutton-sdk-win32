#pragma once

#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonLoggerListener.h"
#include "OpButton/OpButtonTypes.h"

namespace OpButton {

class OpButtonLog {
public:
    static void setLevel(OpButtonLogLevel level);
    static void setLoggerListener(OpButtonLoggerListener* listener);

    static void d(const char* fmt, ...);
    static void i(const char* fmt, ...);
    static void w(const char* fmt, ...);
    static void e(const char* fmt, ...);

private:
    static OpButtonLogLevel s_level;
    static OpButtonLoggerListener* s_listener;
    static const char* TAG;

    static void logMessage(OpButtonLogLevel level, const char* fmt, va_list args);
};

} // namespace OpButton
