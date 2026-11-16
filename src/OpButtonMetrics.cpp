#include "OpButton/OpButtonMetrics.h"
#include "OpButton/OpButtonLog.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::addCollector( OpButtonMetricCollector * _collector )
    {
        std::lock_guard<OpButtonMutex> lock( m_collectorMutex );
        m_collectors.push_back( _collector );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::removeCollector( OpButtonMetricCollector * _collector )
    {
        std::lock_guard<OpButtonMutex> lock( m_collectorMutex );
        m_collectors.erase( std::remove( m_collectors.begin(), m_collectors.end(), _collector ), m_collectors.end() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::record( const OpButtonMetricType & _metricType, double _value )
    {
        this->bufferEntry_( OpButtonMetricEntry( _metricType, _value ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::record( const OpButtonMetricType & _metricType, double _value, int64_t _timestampMs )
    {
        this->bufferEntry_( OpButtonMetricEntry( _metricType, _value, _timestampMs ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::sample()
    {
        OpButtonVector<OpButtonMetricEntry> collected;

        {
            std::lock_guard<OpButtonMutex> lock( m_collectorMutex );
            for( auto * collector : m_collectors )
            {
                try
                {
                    collector->collect( collected );
                }
                catch( ... )
                {
                    OpButtonLog::e( "Metric collector threw exception" );
                }
            }
        }

        for( const auto & entry : collected )
        {
            this->bufferEntry_( entry );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonVector<OpButtonMetricEntry> OpButtonMetrics::drain()
    {
        std::lock_guard<OpButtonMutex> lock( m_bufferMutex );
        if( m_buffer.empty() == true )
        {
            return {};
        }

        int count = std::min( static_cast<int>( m_buffer.size() ), batchSize );
        OpButtonVector<OpButtonMetricEntry> chunk( m_buffer.begin(), m_buffer.begin() + count );
        m_buffer.erase( m_buffer.begin(), m_buffer.begin() + count );
        return chunk;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::requeue( const OpButtonVector<OpButtonMetricEntry> & _entries )
    {
        if( _entries.empty() == true )
        {
            return;
        }
        std::lock_guard<OpButtonMutex> lock( m_bufferMutex );
        m_buffer.insert( m_buffer.begin(), _entries.begin(), _entries.end() );
        while( static_cast<int>( m_buffer.size() ) > bufferSize )
        {
            m_buffer.pop_back();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonMetrics::hasData() const
    {
        std::lock_guard<OpButtonMutex> lock( m_bufferMutex );
        return !m_buffer.empty();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::clear()
    {
        {
            std::lock_guard<OpButtonMutex> lock( m_bufferMutex );
            m_buffer.clear();
        }
        {
            std::lock_guard<OpButtonMutex> lock( m_collectorMutex );
            m_collectors.clear();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonMetrics::bufferEntry_( const OpButtonMetricEntry & _entry )
    {
        std::lock_guard<OpButtonMutex> lock( m_bufferMutex );
        if( static_cast<int>( m_buffer.size() ) >= bufferSize )
        {
            m_buffer.erase( m_buffer.begin() );
        }
        m_buffer.push_back( _entry );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetrics::OpButtonMetrics()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetrics::~OpButtonMetrics()
    {
    }
} // namespace OpButton
