#include "OpButton/OpButtonMetricType.h"

#include <cstring>

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricType::OpButtonMetricType( const char * _key, const char * _unit, OpButtonMetricAggregation _aggregation )
        : m_key( _key )
        , m_unit( _unit )
        , m_aggregation( _aggregation )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    const char * OpButtonMetricType::key() const
    {
        return m_key;
    }

    //////////////////////////////////////////////////////////////////////////
    const char * OpButtonMetricType::unit() const
    {
        return m_unit;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricAggregation OpButtonMetricType::aggregation() const
    {
        return m_aggregation;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonMetricType::operator==( const OpButtonMetricType & _o ) const
    {
        return strcmp( m_key, _o.m_key ) == 0;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonMetricType::operator!=( const OpButtonMetricType & _o ) const
    {
        return strcmp( m_key, _o.m_key ) != 0;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricType OpButtonMetricType::create( const char * _key )
    {
        return OpButtonMetricType( _key, "", OpButtonMetricAggregation::GAUGE );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricType OpButtonMetricType::create( const char * _key, const char * _unit )
    {
        return OpButtonMetricType( _key, _unit, OpButtonMetricAggregation::GAUGE );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricType OpButtonMetricType::create( const char * _key, const char * _unit, OpButtonMetricAggregation _agg )
    {
        return OpButtonMetricType( _key, _unit, _agg );
    }

    const OpButtonMetricType OpButtonMetricType::FPS{ "fps", "", OpButtonMetricAggregation::GAUGE };
    const OpButtonMetricType OpButtonMetricType::FRAME_TIME{ "frame_time", "ms", OpButtonMetricAggregation::GAUGE };
    const OpButtonMetricType OpButtonMetricType::MEMORY{ "memory", "bytes", OpButtonMetricAggregation::GAUGE };
    const OpButtonMetricType OpButtonMetricType::VIDEO_MEMORY{ "video_memory", "bytes", OpButtonMetricAggregation::GAUGE };
    const OpButtonMetricType OpButtonMetricType::CPU_USAGE{ "cpu_usage", "%", OpButtonMetricAggregation::GAUGE };
    const OpButtonMetricType OpButtonMetricType::NETWORK_RX{ "network_rx", "bytes/s", OpButtonMetricAggregation::RATE };
    const OpButtonMetricType OpButtonMetricType::NETWORK_TX{ "network_tx", "bytes/s", OpButtonMetricAggregation::RATE };
    const OpButtonMetricType OpButtonMetricType::DRAW_CALLS{ "draw_calls", "", OpButtonMetricAggregation::GAUGE };
    const OpButtonMetricType OpButtonMetricType::THREAD_COUNT{ "thread_count", "", OpButtonMetricAggregation::GAUGE };

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricType::~OpButtonMetricType()
    {
    }
} // namespace OpButton
