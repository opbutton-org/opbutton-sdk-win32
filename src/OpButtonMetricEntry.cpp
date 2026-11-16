#include "OpButton/OpButtonMetricEntry.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricEntry::OpButtonMetricEntry( const OpButtonMetricType & _type, double _value )
        : m_type( _type )
        , m_value( _value )
        , m_timestampMs( std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count() )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricEntry::OpButtonMetricEntry( const OpButtonMetricType & _type, double _value, int64_t _timestampMs )
        : m_type( _type )
        , m_value( _value )
        , m_timestampMs( _timestampMs )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonMetricType & OpButtonMetricEntry::type() const
    {
        return m_type;
    }

    //////////////////////////////////////////////////////////////////////////
    double OpButtonMetricEntry::value() const
    {
        return m_value;
    }

    //////////////////////////////////////////////////////////////////////////
    int64_t OpButtonMetricEntry::timestampMs() const
    {
        return m_timestampMs;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonMetricEntry::toJson() const
    {
        return OpButtonJson::object()
            .put( "metric_type", OpButtonJson( m_type.key() ) )
            .put( "value", OpButtonJson( m_value ) )
            .put( "client_timestamp", OpButtonJson( m_timestampMs ) )
            .put( "aggregation", OpButtonJson( opbuttonMetricAggregationKey( m_type.aggregation() ) ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetricEntry::~OpButtonMetricEntry()
    {
    }
} // namespace OpButton
