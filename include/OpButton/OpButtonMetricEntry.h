#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonMetricType.h"
#include <cstdint>
#include <chrono>

namespace OpButton {

class OpButtonMetricEntry {
public:
    OpButtonMetricEntry(const OpButtonMetricType& type, double value);
    OpButtonMetricEntry(const OpButtonMetricType& type, double value, int64_t timestampMs);

    const OpButtonMetricType& type() const;
    double value() const;
    int64_t timestampMs() const;

    OpButtonJson toJson() const;

private:
    OpButtonMetricType m_type;
    double m_value;
    int64_t m_timestampMs;
};

} // namespace OpButton
