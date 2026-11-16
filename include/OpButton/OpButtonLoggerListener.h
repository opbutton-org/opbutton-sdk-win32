#pragma once

#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonLoggerListener
    {
    public:
        OpButtonLoggerListener();
        virtual ~OpButtonLoggerListener();

        virtual void onOpButtonLoggerMessage( OpButtonLogLevel _level, const OpButtonString & _tag, const OpButtonString & _message ) = 0;
    };

} // namespace OpButton
