#pragma once

#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonException;

    class OpButtonSessionListener
    {
    public:
        OpButtonSessionListener();
        virtual ~OpButtonSessionListener();

    public:
        virtual void onOpButtonSessionConnecting();
        virtual void onOpButtonSessionWaitingApproval( const OpButtonString & _requestId );
        virtual void onOpButtonSessionStarted( const OpButtonString & _sessionToken );
        virtual void onOpButtonSessionClosed();
        virtual void onOpButtonSessionRejected();
        virtual void onOpButtonSessionAuthFailed();
        virtual void onOpButtonSessionError( const OpButtonException & _exception );
    };

} // namespace OpButton
