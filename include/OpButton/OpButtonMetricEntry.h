#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonMetricType.h"
#include <chrono>
#include <cstdint>

namespace OpButton
{
    class OpButtonMetricEntry
    {
    public:
        OpButtonMetricEntry( const OpButtonMetricType & _type, double _value );
        ~OpButtonMetricEntry();
        OpButtonMetricEntry( const OpButtonMetricType & _type, double _value, int64_t _timestampMs );

        const OpButtonMetricType & type() const;
        double value() const;
        int64_t timestampMs() const;

        OpButtonJson toJson() const;

    private:
        OpButtonMetricType m_type;
        double m_value;
        int64_t m_timestampMs;
    };

} // namespace OpButton
