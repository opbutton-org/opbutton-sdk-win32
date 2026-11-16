#include "OpButton/OpButtonConnectResponse.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonConnectResponse::OpButtonConnectResponse( const OpButtonString & _requestId, const OpButtonString & _status, const OpButtonString & _sessionToken, const OpButtonString & _deviceSecret )
        : m_requestId( _requestId )
        , m_status( _status )
        , m_sessionToken( _sessionToken )
        , m_deviceSecret( _deviceSecret )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConnectResponse::requestId() const
    {
        return m_requestId;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConnectResponse::status() const
    {
        return m_status;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConnectResponse::sessionToken() const
    {
        return m_sessionToken;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConnectResponse::deviceSecret() const
    {
        return m_deviceSecret;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonConnectResponse::isPending() const
    {
        return m_status == "pending";
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonConnectResponse::isApproved() const
    {
        return m_status == "approved";
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonConnectResponse::isRejected() const
    {
        return m_status == "rejected";
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConnectResponse OpButtonConnectResponse::fromJson( const OpButtonJson & _json )
    {
        return OpButtonConnectResponse( _json.optString( "request_id" ), _json.optString( "status" ), _json.optString( "session_token" ), _json.optString( "device_secret" ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConnectResponse::~OpButtonConnectResponse()
    {
    }
} // namespace OpButton
