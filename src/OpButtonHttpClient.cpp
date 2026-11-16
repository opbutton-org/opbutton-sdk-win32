#include "OpButton/OpButtonHttpClient.h"
#include "OpButton/OpButtonException.h"
#include "OpButton/OpButtonLog.h"

#include "OpButtonWin32.h"
#include <wininet.h>
#include <objbase.h>

#include <cstdlib>
#include <cstring>

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    static OpButtonString createRunId()
    {
        GUID guid;
        if( FAILED( CoCreateGuid( &guid ) ) )
        {
            throw OpButtonException( "Failed to generate SDK run ID" );
        }

        wchar_t value[39];
        if( StringFromGUID2( guid, value, 39 ) != 39 )
        {
            throw OpButtonException( "Failed to format SDK run ID" );
        }

        OpButtonString result;
        result.reserve( 36 );
        for( size_t i = 1; i != 37; ++i )
        {
            result.push_back( static_cast<char>( value[i] ) );
        }
        return result;
    }

    //////////////////////////////////////////////////////////////////////////
    static void parseUrl( const OpButtonString & _url, OpButtonString & _host, int & _port, OpButtonString & _basePath, bool & _secure )
    {
        OpButtonString remainder = _url;
        _secure = false;
        _port = 80;

        if( remainder.substr( 0, 8 ) == "https://" )
        {
            _secure = true;
            _port = 443;
            remainder = remainder.substr( 8 );
        }
        else if( remainder.substr( 0, 7 ) == "http://" )
        {
            remainder = remainder.substr( 7 );
        }

        size_t slashPos = remainder.find( '/' );
        OpButtonString hostPort;
        if( slashPos != OpButtonString::npos )
        {
            hostPort = remainder.substr( 0, slashPos );
            _basePath = remainder.substr( slashPos );
        }
        else
        {
            hostPort = remainder;
            _basePath = OpButtonString();
        }

        // Trim trailing slash from basePath
        while( !_basePath.empty() && _basePath.back() == '/' )
        {
            _basePath.pop_back();
        }

        size_t colonPos = hostPort.find( ':' );
        if( colonPos != OpButtonString::npos )
        {
            _host = hostPort.substr( 0, colonPos );
            _port = atoi( hostPort.substr( colonPos + 1 ).c_str() );
        }
        else
        {
            _host = hostPort;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonHttpClient::OpButtonHttpClient( const OpButtonString & _baseUrl, const OpButtonString & _apiToken )
        : m_port( 80 )
        , m_secure( false )
        , m_apiToken( _apiToken )
        , m_runId( createRunId() )
        , m_hSession( nullptr )
        , m_hConnect( nullptr )
    {
        parseUrl( _baseUrl, m_host, m_port, m_basePath, m_secure );

        m_hSession = InternetOpenA( "OpButtonSDK/1.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0 );
        if( m_hSession == nullptr )
        {
            throw OpButtonException( "Failed to initialize WinINet session" );
        }

        m_hConnect = InternetConnectA( static_cast<HINTERNET>( m_hSession ), m_host.c_str(), static_cast<INTERNET_PORT>( m_port ), nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0 );
        if( m_hConnect == nullptr )
        {
            InternetCloseHandle( static_cast<HINTERNET>( m_hSession ) );
            m_hSession = nullptr;
            throw OpButtonException( "Failed to connect to " + m_host );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonHttpClient::~OpButtonHttpClient()
    {
        this->shutdown();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonHttpClient::shutdown()
    {
        if( m_hConnect != nullptr )
        {
            InternetCloseHandle( static_cast<HINTERNET>( m_hConnect ) );
            m_hConnect = nullptr;
        }
        if( m_hSession != nullptr )
        {
            InternetCloseHandle( static_cast<HINTERNET>( m_hSession ) );
            m_hSession = nullptr;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonHttpClient::executeRequest_( const OpButtonString & _method, const OpButtonString & _path, const OpButtonString & _headerName, const OpButtonString & _headerValue, const OpButtonString & _body )
    {
        if( m_hConnect == nullptr )
        {
            throw OpButtonException( "HTTP client is not connected" );
        }

        OpButtonString fullPath = m_basePath + _path;

        DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
        if( m_secure == true )
        {
            flags |= INTERNET_FLAG_SECURE;
        }

        HINTERNET hRequest = HttpOpenRequestA( static_cast<HINTERNET>( m_hConnect ), _method.c_str(), fullPath.c_str(), nullptr, nullptr, nullptr, flags, 0 );

        if( hRequest == nullptr )
        {
            throw OpButtonException( "Failed to create HTTP request" );
        }

        // Add content type header
        OpButtonString contentTypeHeader = "Content-Type: application/json; charset=utf-8\r\n";
        HttpAddRequestHeadersA( hRequest, contentTypeHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD );

        // Add custom header (API token or session token)
        if( _headerName.empty() == false )
        {
            OpButtonString customHeader = _headerName + OpButtonString( ": " ) + _headerValue + OpButtonString( "\r\n" );
            HttpAddRequestHeadersA( hRequest, customHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD );
        }

        // Send request
        BOOL result;
        if( _body.empty() == true )
        {
            result = HttpSendRequestA( hRequest, nullptr, 0, nullptr, 0 );
        }
        else
        {
            result = HttpSendRequestA( hRequest, nullptr, 0, const_cast<char *>( _body.c_str() ), static_cast<DWORD>( _body.size() ) );
        }

        if( result == 0 )
        {
            DWORD err = GetLastError();
            InternetCloseHandle( hRequest );
            throw OpButtonException( "Network error: WinINet error code " + opbuttonToString( static_cast<unsigned long>( err ) ) );
        }

        // Read status code
        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof( statusCode );
        HttpQueryInfoA( hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, nullptr );

        // Read response body
        OpButtonString responseBody;
        char readBuf[4096];
        DWORD bytesRead = 0;
        while( InternetReadFile( hRequest, readBuf, sizeof( readBuf ), &bytesRead ) && bytesRead > 0 )
        {
            responseBody.append( readBuf, bytesRead );
        }

        InternetCloseHandle( hRequest );

        if( statusCode < 200 || statusCode >= 300 )
        {
            OpButtonString detail = responseBody;
            OpButtonJson errorJson = OpButtonJson::parse( responseBody );
            OpButtonString detailStr = errorJson.optString( "detail" );
            if( detailStr.empty() == false )
            {
                detail = detailStr;
            }
            throw OpButtonException( static_cast<int>( statusCode ), OpButtonString( "HTTP " ) + opbuttonToString( statusCode ) + OpButtonString( ": " ) + detail );
        }

        if( responseBody.empty() == true )
        {
            return OpButtonJson::object();
        }
        return OpButtonJson::parse( responseBody );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConnectResponse OpButtonHttpClient::connect( const OpButtonString & _deviceId, const OpButtonString & _deviceName, const OpButtonMap<OpButtonString, OpButtonJson> & _sessionAttributes )
    {
        OpButtonJson body = OpButtonJson::object()
            .put( "device_id", OpButtonJson( _deviceId ) )
            .put( "device_name", OpButtonJson( _deviceName ) )
            .put( "run_id", OpButtonJson( m_runId ) );

        if( _sessionAttributes.empty() == false )
        {
            OpButtonJson attrs = OpButtonJson::object();
            for( const auto & kv : _sessionAttributes )
            {
                attrs.put( kv.first, kv.second );
            }
            body.put( "session_attributes", attrs );
        }

        OpButtonJson resp = this->executeRequest_( "POST", "/api/client/connect", "X-Api-Token", m_apiToken, body.toString() );
        return OpButtonConnectResponse::fromJson( resp );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConnectResponse OpButtonHttpClient::pollStatus( const OpButtonString & _requestId )
    {
        OpButtonJson resp = this->executeRequest_( "GET", "/api/client/poll-status/" + _requestId + "?run_id=" + m_runId, "X-Api-Token", m_apiToken, "" );
        return OpButtonConnectResponse::fromJson( resp );
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonHttpClient::closeSession( const OpButtonString & _sessionToken )
    {
        OpButtonJson resp = this->executeRequest_( "POST", "/api/client/session/close", "X-Session-Token", _sessionToken, "" );
        return resp.optBool( "ok", false );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonHttpClient::submitPanel( const OpButtonString & _sessionToken, const OpButtonJson & _layout )
    {
        OpButtonJson body = OpButtonJson::object().put( "layout", _layout );
        return this->executeRequest_( "POST", "/api/client/session/panel", "X-Session-Token", _sessionToken, body.toString() );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonHttpClient::pollActions( const OpButtonString & _sessionToken, const OpButtonMap<OpButtonString, OpButtonJson> * _changedAttributes, const OpButtonVector<OpButtonLogEntry> & _logs, const OpButtonVector<OpButtonMetricEntry> & _metrics )
    {
        OpButtonJson body = OpButtonJson::object();

        if( _changedAttributes && !_changedAttributes->empty() )
        {
            OpButtonJson attrs = OpButtonJson::object();
            for( const auto & kv : *_changedAttributes )
            {
                attrs.put( kv.first, kv.second );
            }
            body.put( "session_attributes", attrs );
        }

        if( _logs.empty() == false )
        {
            OpButtonJson logsArr = OpButtonJson::array();
            for( const auto & entry : _logs )
            {
                logsArr.add( entry.toJson() );
            }
            body.put( "logs", logsArr );
        }

        if( _metrics.empty() == false )
        {
            OpButtonJson metricsArr = OpButtonJson::array();
            for( const auto & entry : _metrics )
            {
                metricsArr.add( entry.toJson() );
            }
            body.put( "metrics", metricsArr );
        }

        OpButtonString bodyStr = ( body.objectSize() > 0 ) ? body.toString() : OpButtonString();
        return this->executeRequest_( "POST", "/api/client/session/actions/poll", "X-Session-Token", _sessionToken, bodyStr );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonHttpClient::acknowledgeAction( const OpButtonString & _sessionToken, const OpButtonString & _actionId, const OpButtonJson & _ackPayload )
    {
        OpButtonJson body = OpButtonJson::object().put( "action_id", OpButtonJson( _actionId ) ).put( "ack_payload", _ackPayload.isNull() ? OpButtonJson::object() : _ackPayload );

        this->executeRequest_( "POST", "/api/client/session/actions/ack", "X-Session-Token", _sessionToken, body.toString() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonHttpClient::sendLogs( const OpButtonString & _sessionToken, const OpButtonVector<OpButtonLogEntry> & _logs )
    {
        if( _logs.empty() == true )
        {
            return;
        }

        OpButtonJson logsArr = OpButtonJson::array();
        for( const auto & entry : _logs )
        {
            logsArr.add( entry.toJson() );
        }

        OpButtonJson body = OpButtonJson::object().put( "logs", logsArr );
        this->executeRequest_( "POST", "/api/client/session/logs", "X-Session-Token", _sessionToken, body.toString() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonHttpClient::sendMetrics( const OpButtonString & _sessionToken, const OpButtonVector<OpButtonMetricEntry> & _metrics )
    {
        if( _metrics.empty() == true )
        {
            return;
        }

        OpButtonJson metricsArr = OpButtonJson::array();
        for( const auto & entry : _metrics )
        {
            metricsArr.add( entry.toJson() );
        }

        OpButtonJson body = OpButtonJson::object().put( "metrics", metricsArr );
        this->executeRequest_( "POST", "/api/client/session/metrics", "X-Session-Token", _sessionToken, body.toString() );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonHttpClient::getLivePublisherState( const OpButtonString & _sessionToken )
    {
        return this->executeRequest_( "GET", "/api/client/session/live/publisher", "X-Session-Token", _sessionToken, "" );
    }

} // namespace OpButton
