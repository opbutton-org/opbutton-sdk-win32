#pragma once

#include "OpButton/OpButtonMetricCollector.h"
#include "OpButton/OpButtonMetricEntry.h"
#include "OpButton/OpButtonMetricType.h"
#include <cstdint>

namespace OpButton
{
    class OpButtonDefaultMetricCollector : public OpButtonMetricCollector
    {
    public:
        OpButtonDefaultMetricCollector();
        ~OpButtonDefaultMetricCollector() override;
        void collect( OpButtonVector<OpButtonMetricEntry> & _out ) override;

    private:
        void collectMemory_( OpButtonVector<OpButtonMetricEntry> & _out );
        void collectCpu_( OpButtonVector<OpButtonMetricEntry> & _out );

        int64_t m_prevCpuKernel = 0;
        int64_t m_prevCpuUser = 0;
        int64_t m_prevCpuTime = 0;
    };

} // namespace OpButton
