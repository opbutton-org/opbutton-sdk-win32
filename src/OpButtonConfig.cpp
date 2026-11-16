#include "OpButton/OpButtonConfig.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder::Builder( const OpButtonString & _baseUrl, const OpButtonString & _apiToken )
        : m_baseUrl( _baseUrl )
        , m_apiToken( _apiToken )
    {
        while( !m_baseUrl.empty() && m_baseUrl.back() == '/' )
        {
            m_baseUrl.pop_back();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setDeviceId( const OpButtonString & _v )
    {
        m_deviceId = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setDeviceName( const OpButtonString & _v )
    {
        m_deviceName = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setPollIntervalMs( int64_t _v )
    {
        m_pollIntervalMs = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setActionPollIntervalMs( int64_t _v )
    {
        m_actionPollIntervalMs = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setAutoConnect( bool _v )
    {
        m_autoConnect = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setLoggerListener( OpButtonLoggerListener * _v )
    {
        m_loggerListener = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setSessionListener( OpButtonSessionListener * _v )
    {
        m_sessionListener = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setActionListener( OpButtonActionListener * _v )
    {
        m_actionListener = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setSessionAttributes( const OpButtonSessionAttributeBuilder & _v )
    {
        m_sessionAttributes = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setLogConfig( const OpButtonLogConfigBuilder & _v )
    {
        m_logConfig = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setMetricConfig( const OpButtonMetricConfigBuilder & _v )
    {
        m_metricConfig = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig OpButtonConfig::Builder::build() const
    {
        OpButtonConfig c;
        c.m_baseUrl = m_baseUrl;
        c.m_apiToken = m_apiToken;
        c.m_deviceId = m_deviceId;
        c.m_deviceName = m_deviceName;
        c.m_pollIntervalMs = m_pollIntervalMs;
        c.m_actionPollIntervalMs = m_actionPollIntervalMs;
        c.m_autoConnect = m_autoConnect;
        c.m_loggerListener = m_loggerListener;
        c.m_sessionListener = m_sessionListener;
        c.m_actionListener = m_actionListener;
        c.m_sessionAttributes = m_sessionAttributes;
        c.m_logConfig = m_logConfig;
        c.m_metricConfig = m_metricConfig;
        return c;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConfig::baseUrl() const
    {
        return m_baseUrl;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConfig::apiToken() const
    {
        return m_apiToken;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConfig::deviceId() const
    {
        return m_deviceId;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonConfig::deviceName() const
    {
        return m_deviceName;
    }

    //////////////////////////////////////////////////////////////////////////
    int64_t OpButtonConfig::pollIntervalMs() const
    {
        return m_pollIntervalMs;
    }

    //////////////////////////////////////////////////////////////////////////
    int64_t OpButtonConfig::actionPollIntervalMs() const
    {
        return m_actionPollIntervalMs;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonConfig::autoConnect() const
    {
        return m_autoConnect;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLoggerListener * OpButtonConfig::loggerListener() const
    {
        return m_loggerListener;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionListener * OpButtonConfig::sessionListener() const
    {
        return m_sessionListener;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonActionListener * OpButtonConfig::actionListener() const
    {
        return m_actionListener;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonSessionAttributeBuilder & OpButtonConfig::sessionAttributes() const
    {
        return m_sessionAttributes;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonLogConfigBuilder & OpButtonConfig::logConfig() const
    {
        return m_logConfig;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonMetricConfigBuilder & OpButtonConfig::metricConfig() const
    {
        return m_metricConfig;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::OpButtonConfig()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::~OpButtonConfig()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder::~Builder()
    {
    }
} // namespace OpButton
