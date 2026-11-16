#pragma once

#include "OpButton/OpButtonMetricAggregation.h"

namespace OpButton
{
    class OpButtonMetricType
    {
    public:
        OpButtonMetricType( const char * _key, const char * _unit, OpButtonMetricAggregation _aggregation );
        ~OpButtonMetricType();

    public:
        const char * key() const;
        const char * unit() const;
        OpButtonMetricAggregation aggregation() const;

    public:
        bool operator==( const OpButtonMetricType & _o ) const;
        bool operator!=( const OpButtonMetricType & _o ) const;

    public:
        static OpButtonMetricType create( const char * _key );
        static OpButtonMetricType create( const char * _key, const char * _unit );
        static OpButtonMetricType create( const char * _key, const char * _unit, OpButtonMetricAggregation _agg );

    public:
        static const OpButtonMetricType FPS;
        static const OpButtonMetricType FRAME_TIME;
        static const OpButtonMetricType MEMORY;
        static const OpButtonMetricType VIDEO_MEMORY;
        static const OpButtonMetricType CPU_USAGE;
        static const OpButtonMetricType NETWORK_RX;
        static const OpButtonMetricType NETWORK_TX;
        static const OpButtonMetricType DRAW_CALLS;
        static const OpButtonMetricType THREAD_COUNT;

    private:
        const char * m_key;
        const char * m_unit;
        OpButtonMetricAggregation m_aggregation;
    };

} // namespace OpButton
