#include "OpButton/OpButtonSessionListener.h"

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionListener::~OpButtonSessionListener()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSessionListener::onOpButtonSessionConnecting()
    {
        // Empty
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSessionListener::onOpButtonSessionWaitingApproval( const OpButtonString & _requestId )
    {
        (void)_requestId;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSessionListener::onOpButtonSessionStarted( const OpButtonString & _sessionToken )
    {
        (void)_sessionToken;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSessionListener::onOpButtonSessionClosed()
    {
        // Empty
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSessionListener::onOpButtonSessionRejected()
    {
        // Empty
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSessionListener::onOpButtonSessionAuthFailed()
    {
        // Empty
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSessionListener::onOpButtonSessionError( const OpButtonException & _exception )
    {
        (void)_exception;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionListener::OpButtonSessionListener()
    {
    }
} // namespace OpButton
