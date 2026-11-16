#include "OpButton/OpButtonActionStatus.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    const char * opbuttonActionStatusValue( OpButtonActionStatus _s )
    {
        switch( _s )
        {
        case OpButtonActionStatus::PENDING:
            return "pending";
        case OpButtonActionStatus::DELIVERED:
            return "delivered";
        case OpButtonActionStatus::ACKNOWLEDGED:
            return "acknowledged";
        case OpButtonActionStatus::UNKNOWN:
            return "";
        }
        return "";
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonActionStatus opbuttonActionStatusFromValue( const OpButtonString & _v )
    {
        if( _v == "pending" )
        {
            return OpButtonActionStatus::PENDING;
        }
        if( _v == "delivered" )
        {
            return OpButtonActionStatus::DELIVERED;
        }
        if( _v == "acknowledged" )
        {
            return OpButtonActionStatus::ACKNOWLEDGED;
        }
        return OpButtonActionStatus::UNKNOWN;
    }

} // namespace OpButton
