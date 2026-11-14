#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton {

class OpButtonMetricEntry;

class OpButtonMetricCollector {
public:
    virtual ~OpButtonMetricCollector() = default;
    virtual void collect(OpButtonVector<OpButtonMetricEntry>& out) = 0;
};

} // namespace OpButton
