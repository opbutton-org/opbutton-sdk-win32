#pragma once

#include "OpButton/OpButtonJson.h"

namespace OpButton
{
    class OpButtonConnectResponse
    {
    public:
        OpButtonConnectResponse( const OpButtonString & _requestId, const OpButtonString & _status, const OpButtonString & _sessionToken, const OpButtonString & _deviceSecret );
        ~OpButtonConnectResponse();

    public:
        const OpButtonString & requestId() const;
        const OpButtonString & status() const;
        const OpButtonString & sessionToken() const;
        const OpButtonString & deviceSecret() const;

        bool isPending() const;
        bool isApproved() const;
        bool isRejected() const;

        static OpButtonConnectResponse fromJson( const OpButtonJson & _json );

    private:
        OpButtonString m_requestId;
        OpButtonString m_status;
        OpButtonString m_sessionToken;
        OpButtonString m_deviceSecret;
    };

} // namespace OpButton
