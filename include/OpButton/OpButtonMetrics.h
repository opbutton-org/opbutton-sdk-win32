#pragma once

#include "OpButton/OpButtonMetricEntry.h"
#include "OpButton/OpButtonMetricCollector.h"
#include <algorithm>
#include <cstdint>

namespace OpButton {

class OpButtonMetrics {
public:
    OpButtonMetrics() = default;

    void addCollector(OpButtonMetricCollector* collector);
    void removeCollector(OpButtonMetricCollector* collector);

    void record(const OpButtonMetricType& metricType, double value);
    void record(const OpButtonMetricType& metricType, double value, int64_t timestampMs);

    void sample();
    OpButtonVector<OpButtonMetricEntry> drain();
    void requeue(const OpButtonVector<OpButtonMetricEntry>& entries);
    bool hasData() const;
    void clear();

    int64_t sampleIntervalMs = 200;
    int bufferSize = 2000;
    int batchSize = 200;

private:
    void bufferEntry(const OpButtonMetricEntry& entry);

    OpButtonVector<OpButtonMetricCollector*> m_collectors;
    OpButtonMutex m_collectorMutex;

    OpButtonVector<OpButtonMetricEntry> m_buffer;
    mutable OpButtonMutex m_bufferMutex;
};

} // namespace OpButton
