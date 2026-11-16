#include "OpButton/OpButtonHttpClient.h"
#include "OpButton/OpButtonException.h"
#include "OpButton/OpButtonLog.h"

#include "OpButtonWin32.h"
#include <wininet.h>
#include <wincred.h>
#include <objbase.h>

#include <cstdlib>
#include <cstring>
#include <limits>

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
    static std::wstring utf8ToWide( const OpButtonString & _value )
    {
        int size = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, _value.c_str(), static_cast<int>( _value.size() ), nullptr, 0 );
        if( size <= 0 )
        {
            throw OpButtonException( "Invalid UTF-8 in device credential key" );
        }
        std::wstring output( static_cast<size_t>( size ), L'\0' );
        MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, _value.c_str(), static_cast<int>( _value.size() ), output.data(), size );
        return output;
    }

    //////////////////////////////////////////////////////////////////////////
    static void parseUrl( const OpButtonString & _url, OpButtonString & _host, int & _port, OpButtonString & _basePath, bool & _secure )
    {
        char host[INTERNET_MAX_HOST_NAME_LENGTH]{};
        char path[INTERNET_MAX_PATH_LENGTH]{};
        char extra[INTERNET_MAX_PATH_LENGTH]{};
        char user[INTERNET_MAX_USER_NAME_LENGTH]{};
        char password[INTERNET_MAX_PASSWORD_LENGTH]{};
        URL_COMPONENTSA components{};
        components.dwStructSize = sizeof( components );
        components.lpszHostName = host;
        components.dwHostNameLength = sizeof( host );
        components.lpszUrlPath = path;
        components.dwUrlPathLength = sizeof( path );
        components.lpszExtraInfo = extra;
        components.dwExtraInfoLength = sizeof( extra );
        components.lpszUserName = user;
        components.dwUserNameLength = sizeof( user );
        components.lpszPassword = password;
        components.dwPasswordLength = sizeof( password );

        if( InternetCrackUrlA( _url.c_str(), static_cast<DWORD>( _url.size() ), 0, &components ) == FALSE )
        {
            throw OpButtonException( "baseUrl is invalid" );
        }
        if( components.nScheme != INTERNET_SCHEME_HTTP && components.nScheme != INTERNET_SCHEME_HTTPS )
        {
            throw OpButtonException( "baseUrl must use HTTP or HTTPS" );
        }
        if( components.dwHostNameLength == 0 || components.dwUserNameLength != 0
            || components.dwPasswordLength != 0 || components.dwExtraInfoLength != 0 )
        {
            throw OpButtonException( "baseUrl must have a host and must not contain credentials, a query, or a fragment" );
        }

        _host.assign( host, components.dwHostNameLength );
        _basePath.assign( path, components.dwUrlPathLength );
        _secure = components.nScheme == INTERNET_SCHEME_HTTPS;
        _port = static_cast<int>( components.nPort );
        while( !_basePath.empty() && _basePath.back() == '/' )
        {
            _basePath.pop_back();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    static bool safeHeaderValue( const OpButtonString & _value )
    {
        if( _value.empty() == true )
        {
            return false;
        }
        for( unsigned char character : _value )
        {
            if( character < 0x21 || character > 0x7e )
            {
                return false;
            }
        }
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonHttpClient::OpButtonHttpClient( const OpButtonString & _baseUrl, const OpButtonString & _apiToken, const OpButtonString & _deviceSecret, bool _allowInsecureLocalhost )
        : m_port( 80 )
        , m_secure( false )
        , m_apiToken( _apiToken )
        , m_runId( createRunId() )
        , m_deviceSecret( _deviceSecret )
        , m_hSession( nullptr )
        , m_hConnect( nullptr )
    {
        parseUrl( _baseUrl, m_host, m_port, m_basePath, m_secure );
        if( m_apiToken.empty() == true || safeHeaderValue( m_apiToken ) == false )
        {
            throw OpButtonException( "apiToken is invalid" );
        }
        if( m_deviceSecret.empty() == false
            && ( m_deviceSecret.size() < 32 || m_deviceSecret.size() > 256
                || safeHeaderValue( m_deviceSecret ) == false ) )
        {
            throw OpButtonException( "deviceSecret must contain 32 to 256 safe characters" );
        }
        bool local = _stricmp( m_host.c_str(), "localhost" ) == 0
            || m_host == "127.0.0.1" || m_host == "::1" || m_host == "[::1]";
        if( m_secure == false && !( _allowInsecureLocalhost && local ) )
        {
            throw OpButtonException( "baseUrl must use HTTPS; localhost HTTP requires explicit opt-in" );
        }
        m_deviceCredentialScope = ( m_secure ? "https://" : "http://" ) + m_host
            + ":" + opbuttonToString( m_port ) + m_basePath;

        m_hSession = InternetOpenA( "OpButtonSDK/1.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0 );
        if( m_hSession == nullptr )
        {
            throw OpButtonException( "Failed to initialize WinINet session" );
        }
        DWORD timeoutMs = 15000;
        InternetSetOptionA( static_cast<HINTERNET>( m_hSession ), INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMs, sizeof( timeoutMs ) );
        InternetSetOptionA( static_cast<HINTERNET>( m_hSession ), INTERNET_OPTION_SEND_TIMEOUT, &timeoutMs, sizeof( timeoutMs ) );
        InternetSetOptionA( static_cast<HINTERNET>( m_hSession ), INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMs, sizeof( timeoutMs ) );

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
    OpButtonJson OpButtonHttpClient::executeRequest_( const OpButtonString & _method, const OpButtonString & _path, const OpButtonString & _headerName, const OpButtonString & _headerValue, const OpButtonString & _body, const OpButtonString & _secondHeaderName, const OpButtonString & _secondHeaderValue )
    {
        if( m_hConnect == nullptr )
        {
            throw OpButtonException( "HTTP client is not connected" );
        }

        OpButtonString fullPath = m_basePath + _path;

        DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_AUTO_REDIRECT;
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
        if( HttpAddRequestHeadersA( hRequest, contentTypeHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD ) == FALSE )
        {
            InternetCloseHandle( hRequest );
            throw OpButtonException( "Failed to add HTTP content type header" );
        }

        // Add custom header (API token or session token)
        if( _headerName.empty() == false )
        {
            if( safeHeaderValue( _headerName ) == false || safeHeaderValue( _headerValue ) == false )
            {
                InternetCloseHandle( hRequest );
                throw OpButtonException( "Invalid HTTP header value" );
            }
            OpButtonString customHeader = _headerName + OpButtonString( ": " ) + _headerValue + OpButtonString( "\r\n" );
            if( HttpAddRequestHeadersA( hRequest, customHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD ) == FALSE )
            {
                InternetCloseHandle( hRequest );
                throw OpButtonException( "Failed to add HTTP authentication header" );
            }
        }
        if( _secondHeaderName.empty() == false )
        {
            if( safeHeaderValue( _secondHeaderName ) == false || safeHeaderValue( _secondHeaderValue ) == false )
            {
                InternetCloseHandle( hRequest );
                throw OpButtonException( "Invalid HTTP header value" );
            }
            OpButtonString customHeader = _secondHeaderName + OpButtonString( ": " ) + _secondHeaderValue + OpButtonString( "\r\n" );
            if( HttpAddRequestHeadersA( hRequest, customHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD ) == FALSE )
            {
                InternetCloseHandle( hRequest );
                throw OpButtonException( "Failed to add HTTP device header" );
            }
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
        constexpr size_t maxResponseBytes = 2 * 1024 * 1024;
        DWORD contentLength = 0;
        DWORD contentLengthSize = sizeof( contentLength );
        if( HttpQueryInfoA( hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
            &contentLength, &contentLengthSize, nullptr ) != FALSE && contentLength > maxResponseBytes )
        {
            InternetCloseHandle( hRequest );
            throw OpButtonException( "Server response exceeds 2 MiB" );
        }
        char readBuf[4096];
        for( ;; )
        {
            DWORD bytesRead = 0;
            if( InternetReadFile( hRequest, readBuf, sizeof( readBuf ), &bytesRead ) == FALSE )
            {
                DWORD err = GetLastError();
                InternetCloseHandle( hRequest );
                throw OpButtonException( "Network read error: WinINet error code "
                    + opbuttonToString( static_cast<unsigned long>( err ) ) );
            }
            if( bytesRead == 0 )
            {
                break;
            }
            if( responseBody.size() > maxResponseBytes - bytesRead )
            {
                InternetCloseHandle( hRequest );
                throw OpButtonException( "Server response exceeds 2 MiB" );
            }
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
        this->ensureDeviceSecret_( _deviceId );
        OpButtonJson body = OpButtonJson::object()
            .put( "device_id", OpButtonJson( _deviceId ) )
            .put( "device_name", OpButtonJson( _deviceName ) )
            .put( "run_id", OpButtonJson( m_runId ) )
            .put( "device_secret", OpButtonJson( m_deviceSecret ) );

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
        OpButtonConnectResponse response = OpButtonConnectResponse::fromJson( resp );
        if( response.deviceSecret().empty() == false )
        {
            if( response.deviceSecret().size() < 32 || response.deviceSecret().size() > 256
                || safeHeaderValue( response.deviceSecret() ) == false )
            {
                throw OpButtonException( "Server returned an invalid device credential" );
            }
            m_deviceSecret = response.deviceSecret();
            this->persistDeviceSecret_( _deviceId );
        }
        return response;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConnectResponse OpButtonHttpClient::pollStatus( const OpButtonString & _requestId )
    {
        OpButtonJson resp = this->executeRequest_( "GET", "/api/client/poll-status/" + _requestId + "?run_id=" + m_runId, "X-Api-Token", m_apiToken, "", "X-Device-Secret", m_deviceSecret );
        return OpButtonConnectResponse::fromJson( resp );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonHttpClient::ensureDeviceSecret_( const OpButtonString & _deviceId )
    {
        if( m_deviceSecret.empty() == false )
        {
            return;
        }
        m_deviceCredentialTarget = "OpButton/" + m_deviceCredentialScope + "/" + _deviceId;
        std::wstring target = utf8ToWide( m_deviceCredentialTarget );
        PCREDENTIALW credential = nullptr;
        if( CredReadW( target.c_str(), CRED_TYPE_GENERIC, 0, &credential ) != FALSE )
        {
            m_deviceSecret.assign( reinterpret_cast<const char *>( credential->CredentialBlob ), credential->CredentialBlobSize );
            CredFree( credential );
            if( m_deviceSecret.size() >= 32 && m_deviceSecret.size() <= 256
                && safeHeaderValue( m_deviceSecret ) == true )
            {
                return;
            }
            throw OpButtonException( "Stored device credential is invalid" );
        }
        DWORD credentialError = GetLastError();
        if( credentialError != ERROR_NOT_FOUND )
        {
            throw OpButtonException( "Unable to read device credential from Windows Credential Manager" );
        }
        m_deviceSecret = createRunId() + createRunId();
        this->persistDeviceSecret_( _deviceId );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonHttpClient::persistDeviceSecret_( const OpButtonString & _deviceId )
    {
        if( m_deviceCredentialTarget.empty() == true )
        {
            m_deviceCredentialTarget = "OpButton/" + m_deviceCredentialScope + "/" + _deviceId;
        }
        std::wstring target = utf8ToWide( m_deviceCredentialTarget );
        CREDENTIALW credential{};
        credential.Type = CRED_TYPE_GENERIC;
        credential.TargetName = const_cast<wchar_t *>( target.c_str() );
        credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
        credential.CredentialBlobSize = static_cast<DWORD>( m_deviceSecret.size() );
        credential.CredentialBlob = reinterpret_cast<LPBYTE>( m_deviceSecret.data() );
        if( CredWriteW( &credential, 0 ) == FALSE )
        {
            throw OpButtonException( "Unable to persist device credential in Windows Credential Manager" );
        }
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
