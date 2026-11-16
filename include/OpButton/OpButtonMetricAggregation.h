#pragma once

namespace OpButton
{
    enum class OpButtonMetricAggregation
    {
        GAUGE,
        COUNTER,
        RATE
    };

    const char * opbuttonMetricAggregationKey( OpButtonMetricAggregation _a );

} // namespace OpButton
