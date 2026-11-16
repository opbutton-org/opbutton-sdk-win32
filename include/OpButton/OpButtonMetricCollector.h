#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonMetricEntry;

    class OpButtonMetricCollector
    {
    public:
        OpButtonMetricCollector();
        virtual ~OpButtonMetricCollector();

    public:
        virtual void collect( OpButtonVector<OpButtonMetricEntry> & _out ) = 0;
    };

} // namespace OpButton
