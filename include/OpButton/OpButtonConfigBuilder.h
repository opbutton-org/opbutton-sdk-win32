#pragma once

#include "OpButton/OpButtonConfig.h"

namespace OpButton
{
    class OpButtonConfig::Builder
    {
    public:
        Builder( const OpButtonString & _baseUrl, const OpButtonString & _apiToken );
        ~Builder();

    public:
        Builder & setDeviceId( const OpButtonString & _v );
        Builder & setDeviceName( const OpButtonString & _v );
        Builder & setDeviceSecret( const OpButtonString & _v );
        Builder & setAllowInsecureLocalhost( bool _v );
        Builder & setPollIntervalMs( int64_t _v );
        Builder & setActionPollIntervalMs( int64_t _v );
        Builder & setAutoConnect( bool _v );
        Builder & setLoggerListener( OpButtonLoggerListener * _v );
        Builder & setSessionListener( OpButtonSessionListener * _v );
        Builder & setActionListener( OpButtonActionListener * _v );
        Builder & setSessionAttributes( const OpButtonSessionAttributeBuilder & _v );
        Builder & setLogConfig( const OpButtonLogConfigBuilder & _v );
        Builder & setMetricConfig( const OpButtonMetricConfigBuilder & _v );

        OpButtonConfig build() const;

    private:
        OpButtonString m_baseUrl;
        OpButtonString m_apiToken;
        OpButtonString m_deviceId;
        OpButtonString m_deviceName;
        OpButtonString m_deviceSecret;
        int64_t m_pollIntervalMs = 10000;
        int64_t m_actionPollIntervalMs = 2000;
        bool m_autoConnect = true;
        bool m_allowInsecureLocalhost = false;
        OpButtonLoggerListener * m_loggerListener = nullptr;
        OpButtonSessionListener * m_sessionListener = nullptr;
        OpButtonActionListener * m_actionListener = nullptr;
        OpButtonSessionAttributeBuilder m_sessionAttributes;
        OpButtonLogConfigBuilder m_logConfig;
        OpButtonMetricConfigBuilder m_metricConfig;
    };
} // namespace OpButton
