#pragma once

#include "OpButton/OpButtonConnectResponse.h"
#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonLogEntry.h"
#include "OpButton/OpButtonMetricEntry.h"

namespace OpButton
{
    class OpButtonHttpClient
    {
    public:
        OpButtonHttpClient( const OpButtonString & _baseUrl, const OpButtonString & _apiToken, const OpButtonString & _deviceSecret = OpButtonString(), bool _allowInsecureLocalhost = false );
        ~OpButtonHttpClient();

    public:
        void shutdown();

        OpButtonConnectResponse connect( const OpButtonString & _deviceId, const OpButtonString & _deviceName, const OpButtonMap<OpButtonString, OpButtonJson> & _sessionAttributes );
        OpButtonConnectResponse pollStatus( const OpButtonString & _requestId );
        bool closeSession( const OpButtonString & _sessionToken );
        OpButtonJson submitPanel( const OpButtonString & _sessionToken, const OpButtonJson & _layout );
        OpButtonJson pollActions( const OpButtonString & _sessionToken, const OpButtonMap<OpButtonString, OpButtonJson> * _changedAttributes, const OpButtonVector<OpButtonLogEntry> & _logs, const OpButtonVector<OpButtonMetricEntry> & _metrics );
        void acknowledgeAction( const OpButtonString & _sessionToken, const OpButtonString & _actionId, const OpButtonJson & _ackPayload );
        void sendLogs( const OpButtonString & _sessionToken, const OpButtonVector<OpButtonLogEntry> & _logs );
        void sendMetrics( const OpButtonString & _sessionToken, const OpButtonVector<OpButtonMetricEntry> & _metrics );
        OpButtonJson getLivePublisherState( const OpButtonString & _sessionToken );

    private:
        OpButtonJson executeRequest_( const OpButtonString & _method, const OpButtonString & _path, const OpButtonString & _headerName, const OpButtonString & _headerValue, const OpButtonString & _body, const OpButtonString & _secondHeaderName = OpButtonString(), const OpButtonString & _secondHeaderValue = OpButtonString() );
        void ensureDeviceSecret_( const OpButtonString & _deviceId );
        void persistDeviceSecret_( const OpButtonString & _deviceId );

    private:
        OpButtonString m_host;
        int m_port;
        OpButtonString m_basePath;
        bool m_secure;
        OpButtonString m_apiToken;
        OpButtonString m_runId;
        OpButtonString m_deviceSecret;
        OpButtonString m_deviceCredentialScope;
        OpButtonString m_deviceCredentialTarget;

        void * m_hSession; // HINTERNET
        void * m_hConnect; // HINTERNET
    };

} // namespace OpButton
