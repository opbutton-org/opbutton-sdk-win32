#include "OpButton/OpButtonLog.h"

#include <cstdarg>
#include <cstdio>

#ifdef _WIN32
#include "OpButtonWin32.h"
#endif

namespace OpButton
{

    OpButtonLogLevel OpButtonLog::s_level = OpButtonLogLevel::PL_INFO;
    OpButtonLoggerListener * OpButtonLog::s_listener = nullptr;
    const char * OpButtonLog::TAG = "OpButtonSDK";

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLog::setLevel( OpButtonLogLevel _level )
    {
        s_level = _level;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLog::setLoggerListener( OpButtonLoggerListener * _listener )
    {
        s_listener = _listener;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLog::logMessage( OpButtonLogLevel _level, const char * _fmt, va_list _args )
    {
        if( static_cast<int>( _level ) < static_cast<int>( s_level ) )
        {
            return;
        }

        char buf[1024];
        vsnprintf( buf, sizeof( buf ), _fmt, _args );

        if( s_listener != nullptr )
        {
            s_listener->onOpButtonLoggerMessage( _level, TAG, buf );
            return;
        }

        const char * levelStr = opbuttonLogLevelValue( _level );

#ifdef _WIN32
        char dbgBuf[1100];
        snprintf( dbgBuf, sizeof( dbgBuf ), "[%s] %s: %s\n", levelStr, TAG, buf );
        OutputDebugStringA( dbgBuf );
#endif

        fprintf( stderr, "[%s] %s: %s\n", levelStr, TAG, buf );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLog::d( const char * _fmt, ... )
    {
        va_list args;
        va_start( args, _fmt );
        logMessage( OpButtonLogLevel::PL_DEBUG, _fmt, args );
        va_end( args );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLog::i( const char * _fmt, ... )
    {
        va_list args;
        va_start( args, _fmt );
        logMessage( OpButtonLogLevel::PL_INFO, _fmt, args );
        va_end( args );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLog::w( const char * _fmt, ... )
    {
        va_list args;
        va_start( args, _fmt );
        logMessage( OpButtonLogLevel::PL_WARNING, _fmt, args );
        va_end( args );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLog::e( const char * _fmt, ... )
    {
        va_list args;
        va_start( args, _fmt );
        logMessage( OpButtonLogLevel::PL_ERROR, _fmt, args );
        va_end( args );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLog::OpButtonLog()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLog::~OpButtonLog()
    {
    }
} // namespace OpButton
