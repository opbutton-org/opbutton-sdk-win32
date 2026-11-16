#include "OpButton/OpButtonConfig.h"

#include <stdexcept>

namespace OpButton
{
    namespace
    {
        bool isSafeHeaderValue( const OpButtonString & _value )
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
    }

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
    OpButtonConfig::Builder & OpButtonConfig::Builder::setDeviceSecret( const OpButtonString & _v )
    {
        m_deviceSecret = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonConfig::Builder & OpButtonConfig::Builder::setAllowInsecureLocalhost( bool _v )
    {
        m_allowInsecureLocalhost = _v;
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
        if( m_baseUrl.empty() == true )
        {
            throw std::invalid_argument( "baseUrl is required" );
        }
        if( isSafeHeaderValue( m_apiToken ) == false )
        {
            throw std::invalid_argument( "apiToken is required and must be safe for an HTTP header" );
        }
        if( m_pollIntervalMs < 250 || m_pollIntervalMs > 300000 || m_actionPollIntervalMs < 250 || m_actionPollIntervalMs > 300000 )
        {
            throw std::invalid_argument( "poll intervals must be between 250ms and 300000ms" );
        }
        if( m_deviceSecret.empty() == false
            && ( m_deviceSecret.size() < 32 || m_deviceSecret.size() > 256
                || isSafeHeaderValue( m_deviceSecret ) == false ) )
        {
            throw std::invalid_argument( "deviceSecret must contain 32 to 256 safe characters" );
        }
        OpButtonConfig c;
        c.m_baseUrl = m_baseUrl;
        c.m_apiToken = m_apiToken;
        c.m_deviceId = m_deviceId;
        c.m_deviceName = m_deviceName;
        c.m_deviceSecret = m_deviceSecret;
        c.m_pollIntervalMs = m_pollIntervalMs;
        c.m_actionPollIntervalMs = m_actionPollIntervalMs;
        c.m_autoConnect = m_autoConnect;
        c.m_allowInsecureLocalhost = m_allowInsecureLocalhost;
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
    const OpButtonString & OpButtonConfig::deviceSecret() const
    {
        return m_deviceSecret;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonConfig::allowInsecureLocalhost() const
    {
        return m_allowInsecureLocalhost;
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
