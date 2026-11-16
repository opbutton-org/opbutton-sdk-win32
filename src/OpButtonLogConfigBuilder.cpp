#include "OpButton/OpButtonLogConfigBuilder.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogConfigBuilder & OpButtonLogConfigBuilder::setEnabled( bool _v )
    {
        m_enabled = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogConfigBuilder & OpButtonLogConfigBuilder::setLogLevel( OpButtonLogLevel _v )
    {
        m_logLevel = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogConfigBuilder & OpButtonLogConfigBuilder::setBatchSize( int _v )
    {
        m_batchSize = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogConfigBuilder & OpButtonLogConfigBuilder::setBufferSize( int _v )
    {
        m_bufferSize = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogConfigBuilder & OpButtonLogConfigBuilder::setAttributes( const OpButtonLogAttributeBuilder & _v )
    {
        m_attributes = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonLogConfigBuilder::isEnabled() const
    {
        return m_enabled;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogLevel OpButtonLogConfigBuilder::logLevel() const
    {
        return m_logLevel;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonLogConfigBuilder::batchSize() const
    {
        return m_batchSize;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonLogConfigBuilder::bufferSize() const
    {
        return m_bufferSize;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonLogAttributeBuilder & OpButtonLogConfigBuilder::attributes() const
    {
        return m_attributes;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogConfigBuilder::OpButtonLogConfigBuilder()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogConfigBuilder::~OpButtonLogConfigBuilder()
    {
    }
} // namespace OpButton
