#pragma once

#include "OpButton/OpButtonMetricCollector.h"
#include <algorithm>
#include <cstdint>

namespace OpButton
{
    class OpButtonMetricConfigBuilder
    {
    public:
        OpButtonMetricConfigBuilder();
        ~OpButtonMetricConfigBuilder();

    public:
        OpButtonMetricConfigBuilder & setEnabled( bool _v );
        OpButtonMetricConfigBuilder & setSampleIntervalMs( int64_t _ms );
        OpButtonMetricConfigBuilder & setBufferSize( int _v );
        OpButtonMetricConfigBuilder & setBatchSize( int _v );
        OpButtonMetricConfigBuilder & addCollector( OpButtonMetricCollector * _c );

        bool isEnabled() const;
        int64_t sampleIntervalMs() const;
        int bufferSize() const;
        int batchSize() const;
        const OpButtonVector<OpButtonMetricCollector *> & collectors() const;

    private:
        bool m_enabled = true;
        int64_t m_sampleIntervalMs = 200;
        int m_bufferSize = 2000;
        int m_batchSize = 200;
        OpButtonVector<OpButtonMetricCollector *> m_collectors;
    };

} // namespace OpButton
