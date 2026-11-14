#include "OpButton/OpButtonMetricEntry.h"

namespace OpButton {

OpButtonMetricEntry::OpButtonMetricEntry(const OpButtonMetricType& type, double value)
    : m_type(type), m_value(value)
    , m_timestampMs(std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch()).count()) {
}

OpButtonMetricEntry::OpButtonMetricEntry(const OpButtonMetricType& type, double value, int64_t timestampMs)
    : m_type(type), m_value(value), m_timestampMs(timestampMs) {
}

const OpButtonMetricType& OpButtonMetricEntry::type() const { return m_type; }
double OpButtonMetricEntry::value() const { return m_value; }
int64_t OpButtonMetricEntry::timestampMs() const { return m_timestampMs; }

OpButtonJson OpButtonMetricEntry::toJson() const {
    return OpButtonJson::object()
        .put("metric_type", OpButtonJson(m_type.key()))
        .put("value", OpButtonJson(m_value))
        .put("client_timestamp", OpButtonJson(m_timestampMs))
        .put("aggregation", OpButtonJson(opbuttonMetricAggregationKey(m_type.aggregation())));
}

} // namespace OpButton
