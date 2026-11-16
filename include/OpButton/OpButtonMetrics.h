#pragma once

#include "OpButton/OpButtonMetricCollector.h"
#include "OpButton/OpButtonMetricEntry.h"
#include <algorithm>
#include <cstdint>

namespace OpButton
{
    class OpButtonMetrics
    {
    public:
        OpButtonMetrics();
        ~OpButtonMetrics();

    public:
        void addCollector( OpButtonMetricCollector * _collector );
        void removeCollector( OpButtonMetricCollector * _collector );

        void record( const OpButtonMetricType & _metricType, double _value );
        void record( const OpButtonMetricType & _metricType, double _value, int64_t _timestampMs );

        void sample();
        OpButtonVector<OpButtonMetricEntry> drain();
        void requeue( const OpButtonVector<OpButtonMetricEntry> & _entries );
        bool hasData() const;
        void clear();

    public:
        int64_t sampleIntervalMs = 200;
        int bufferSize = 2000;
        int batchSize = 200;

    private:
        void bufferEntry_( const OpButtonMetricEntry & _entry );

    private:
        OpButtonVector<OpButtonMetricCollector *> m_collectors;
        OpButtonMutex m_collectorMutex;

        OpButtonVector<OpButtonMetricEntry> m_buffer;
        mutable OpButtonMutex m_bufferMutex;
    };

} // namespace OpButton
