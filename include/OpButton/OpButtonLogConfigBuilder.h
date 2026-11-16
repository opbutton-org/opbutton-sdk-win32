#pragma once

#include "OpButton/OpButtonLogAttributeBuilder.h"
#include "OpButton/OpButtonLogLevel.h"

namespace OpButton
{
    class OpButtonLogConfigBuilder
    {
    public:
        OpButtonLogConfigBuilder();
        ~OpButtonLogConfigBuilder();

    public:
        OpButtonLogConfigBuilder & setEnabled( bool _v );
        OpButtonLogConfigBuilder & setLogLevel( OpButtonLogLevel _v );
        OpButtonLogConfigBuilder & setBatchSize( int _v );
        OpButtonLogConfigBuilder & setBufferSize( int _v );
        OpButtonLogConfigBuilder & setAttributes( const OpButtonLogAttributeBuilder & _v );

        bool isEnabled() const;
        OpButtonLogLevel logLevel() const;
        int batchSize() const;
        int bufferSize() const;
        const OpButtonLogAttributeBuilder & attributes() const;

    private:
        bool m_enabled = true;
        OpButtonLogLevel m_logLevel = OpButtonLogLevel::PL_INFO;
        int m_batchSize = 100;
        int m_bufferSize = 1000;
        OpButtonLogAttributeBuilder m_attributes;
    };

} // namespace OpButton
