#include "OpButton/OpButtonMetricConfigBuilder.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricConfigBuilder & OpButtonMetricConfigBuilder::setEnabled( bool _v )
    {
        m_enabled = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricConfigBuilder & OpButtonMetricConfigBuilder::setSampleIntervalMs( int64_t _ms )
    {
        m_sampleIntervalMs = std::max( int64_t( 100 ), std::min( _ms, int64_t( 1000 ) ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricConfigBuilder & OpButtonMetricConfigBuilder::setBufferSize( int _v )
    {
        m_bufferSize = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricConfigBuilder & OpButtonMetricConfigBuilder::setBatchSize( int _v )
    {
        m_batchSize = _v;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricConfigBuilder & OpButtonMetricConfigBuilder::addCollector( OpButtonMetricCollector * _c )
    {
        m_collectors.push_back( _c );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonMetricConfigBuilder::isEnabled() const
    {
        return m_enabled;
    }

    //////////////////////////////////////////////////////////////////////////
    int64_t OpButtonMetricConfigBuilder::sampleIntervalMs() const
    {
        return m_sampleIntervalMs;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonMetricConfigBuilder::bufferSize() const
    {
        return m_bufferSize;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonMetricConfigBuilder::batchSize() const
    {
        return m_batchSize;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonVector<OpButtonMetricCollector *> & OpButtonMetricConfigBuilder::collectors() const
    {
        return m_collectors;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricConfigBuilder::OpButtonMetricConfigBuilder()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricConfigBuilder::~OpButtonMetricConfigBuilder()
    {
    }
} // namespace OpButton
