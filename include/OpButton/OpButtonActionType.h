#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    enum class OpButtonActionType
    {
        CLICK,
        CHANGE,
        TOGGLE,
        LIVE_START,
        LIVE_UPDATE,
        LIVE_STOP,
        LIVE_TAP,
        LIVE_LONG_PRESS,
        UNKNOWN
    };

    const char * opbuttonActionTypeValue( OpButtonActionType _t );
    OpButtonActionType opbuttonActionTypeFromValue( const OpButtonString & _v );

} // namespace OpButton
