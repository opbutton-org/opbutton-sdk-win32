#include "OpButton/OpButtonLogLevel.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    const char * opbuttonLogLevelValue( OpButtonLogLevel _level )
    {
        switch( _level )
        {
        case OpButtonLogLevel::PL_DEBUG:
            return "debug";
        case OpButtonLogLevel::PL_INFO:
            return "info";
        case OpButtonLogLevel::PL_WARNING:
            return "warning";
        case OpButtonLogLevel::PL_ERROR:
            return "error";
        case OpButtonLogLevel::PL_CRITICAL:
            return "critical";
        case OpButtonLogLevel::PL_EXCEPTION:
            return "exception";
        }
        return "info";
    }

} // namespace OpButton
