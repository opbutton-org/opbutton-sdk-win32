#pragma once

#include "OpButton/OpButtonMetricAggregation.h"

namespace OpButton {

class OpButtonMetricType {
public:
    OpButtonMetricType(const char* key, const char* unit, OpButtonMetricAggregation aggregation);

    const char* key() const;
    const char* unit() const;
    OpButtonMetricAggregation aggregation() const;

    bool operator==(const OpButtonMetricType& o) const;
    bool operator!=(const OpButtonMetricType& o) const;

    static OpButtonMetricType create(const char* key);
    static OpButtonMetricType create(const char* key, const char* unit);
    static OpButtonMetricType create(const char* key, const char* unit, OpButtonMetricAggregation agg);

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
    const char* m_key;
    const char* m_unit;
    OpButtonMetricAggregation m_aggregation;
};

} // namespace OpButton
