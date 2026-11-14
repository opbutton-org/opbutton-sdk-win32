#pragma once

#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonTypes.h"

namespace OpButton {

class OpButtonException;

class OpButtonSessionListener {
public:
    virtual ~OpButtonSessionListener() = default;
    virtual void onOpButtonSessionConnecting() {}
    virtual void onOpButtonSessionWaitingApproval(const OpButtonString& requestId) { (void)requestId; }
    virtual void onOpButtonSessionStarted(const OpButtonString& sessionToken) { (void)sessionToken; }
    virtual void onOpButtonSessionClosed() {}
    virtual void onOpButtonSessionRejected() {}
    virtual void onOpButtonSessionAuthFailed() {}
    virtual void onOpButtonSessionError(const OpButtonException& exception) { (void)exception; }
};

} // namespace OpButton
