#pragma once

#include "OpButton/OpButtonMetricCollector.h"
#include "OpButton/OpButtonMetricEntry.h"
#include "OpButton/OpButtonMetricType.h"
#include <cstdint>

namespace OpButton {

class OpButtonDefaultMetricCollector : public OpButtonMetricCollector {
public:
    OpButtonDefaultMetricCollector();
    void collect(OpButtonVector<OpButtonMetricEntry>& out) override;

private:
    void collectMemory(OpButtonVector<OpButtonMetricEntry>& out);
    void collectCpu(OpButtonVector<OpButtonMetricEntry>& out);

    int64_t m_prevCpuKernel = 0;
    int64_t m_prevCpuUser = 0;
    int64_t m_prevCpuTime = 0;
};

} // namespace OpButton
