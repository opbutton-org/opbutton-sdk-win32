#pragma once

#include "OpButton/OpButtonMetricCollector.h"
#include <cstdint>
#include <algorithm>

namespace OpButton {

class OpButtonMetricConfigBuilder {
public:
    OpButtonMetricConfigBuilder& setEnabled(bool v);
    OpButtonMetricConfigBuilder& setSampleIntervalMs(int64_t ms);
    OpButtonMetricConfigBuilder& setBufferSize(int v);
    OpButtonMetricConfigBuilder& setBatchSize(int v);
    OpButtonMetricConfigBuilder& addCollector(OpButtonMetricCollector* c);

    bool isEnabled() const;
    int64_t sampleIntervalMs() const;
    int bufferSize() const;
    int batchSize() const;
    const OpButtonVector<OpButtonMetricCollector*>& collectors() const;

private:
    bool m_enabled = true;
    int64_t m_sampleIntervalMs = 200;
    int m_bufferSize = 2000;
    int m_batchSize = 200;
    OpButtonVector<OpButtonMetricCollector*> m_collectors;
};

} // namespace OpButton
