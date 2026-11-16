#pragma once

#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonLoggerListener.h"
#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonLog
    {
    public:
        OpButtonLog();
        ~OpButtonLog();

    public:
        static void setLevel( OpButtonLogLevel _level );
        static void setLoggerListener( OpButtonLoggerListener * _listener );

        static void d( const char * _fmt, ... );
        static void i( const char * _fmt, ... );
        static void w( const char * _fmt, ... );
        static void e( const char * _fmt, ... );

    private:
        static OpButtonLogLevel s_level;
        static OpButtonLoggerListener * s_listener;
        static const char * TAG;

    private:
        static void logMessage( OpButtonLogLevel _level, const char * _fmt, va_list _args );
    };

} // namespace OpButton
