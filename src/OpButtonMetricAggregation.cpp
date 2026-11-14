#include "OpButton/OpButtonMetricAggregation.h"

namespace OpButton {

const char* opbuttonMetricAggregationKey(OpButtonMetricAggregation a) {
    switch (a) {
        case OpButtonMetricAggregation::GAUGE: return "gauge";
        case OpButtonMetricAggregation::COUNTER: return "counter";
        case OpButtonMetricAggregation::RATE: return "rate";
    }
    return "gauge";
}

} // namespace OpButton
