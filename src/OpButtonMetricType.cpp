#include "OpButton/OpButtonMetricType.h"

#include <cstring>

namespace OpButton {

OpButtonMetricType::OpButtonMetricType(const char* key, const char* unit, OpButtonMetricAggregation aggregation)
    : m_key(key), m_unit(unit), m_aggregation(aggregation) {
}

const char* OpButtonMetricType::key() const {
    return m_key;
}

const char* OpButtonMetricType::unit() const {
    return m_unit;
}

OpButtonMetricAggregation OpButtonMetricType::aggregation() const {
    return m_aggregation;
}

bool OpButtonMetricType::operator==(const OpButtonMetricType& o) const {
    return strcmp(m_key, o.m_key) == 0;
}

bool OpButtonMetricType::operator!=(const OpButtonMetricType& o) const {
    return strcmp(m_key, o.m_key) != 0;
}

OpButtonMetricType OpButtonMetricType::create(const char* key) {
    return OpButtonMetricType(key, "", OpButtonMetricAggregation::GAUGE);
}

OpButtonMetricType OpButtonMetricType::create(const char* key, const char* unit) {
    return OpButtonMetricType(key, unit, OpButtonMetricAggregation::GAUGE);
}

OpButtonMetricType OpButtonMetricType::create(const char* key, const char* unit, OpButtonMetricAggregation agg) {
    return OpButtonMetricType(key, unit, agg);
}

const OpButtonMetricType OpButtonMetricType::FPS{"fps", "", OpButtonMetricAggregation::GAUGE};
const OpButtonMetricType OpButtonMetricType::FRAME_TIME{"frame_time", "ms", OpButtonMetricAggregation::GAUGE};
const OpButtonMetricType OpButtonMetricType::MEMORY{"memory", "bytes", OpButtonMetricAggregation::GAUGE};
const OpButtonMetricType OpButtonMetricType::VIDEO_MEMORY{"video_memory", "bytes", OpButtonMetricAggregation::GAUGE};
const OpButtonMetricType OpButtonMetricType::CPU_USAGE{"cpu_usage", "%", OpButtonMetricAggregation::GAUGE};
const OpButtonMetricType OpButtonMetricType::NETWORK_RX{"network_rx", "bytes/s", OpButtonMetricAggregation::RATE};
const OpButtonMetricType OpButtonMetricType::NETWORK_TX{"network_tx", "bytes/s", OpButtonMetricAggregation::RATE};
const OpButtonMetricType OpButtonMetricType::DRAW_CALLS{"draw_calls", "", OpButtonMetricAggregation::GAUGE};
const OpButtonMetricType OpButtonMetricType::THREAD_COUNT{"thread_count", "", OpButtonMetricAggregation::GAUGE};

} // namespace OpButton
