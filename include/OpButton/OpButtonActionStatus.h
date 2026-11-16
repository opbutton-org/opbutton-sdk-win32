#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    enum class OpButtonActionStatus
    {
        PENDING,
        DELIVERED,
        ACKNOWLEDGED,
        UNKNOWN
    };

    const char * opbuttonActionStatusValue( OpButtonActionStatus _s );
    OpButtonActionStatus opbuttonActionStatusFromValue( const OpButtonString & _v );

} // namespace OpButton
