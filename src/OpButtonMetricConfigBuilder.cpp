#include "OpButton/OpButtonMetricConfigBuilder.h"

namespace OpButton {

OpButtonMetricConfigBuilder& OpButtonMetricConfigBuilder::setEnabled(bool v) { m_enabled = v; return *this; }

OpButtonMetricConfigBuilder& OpButtonMetricConfigBuilder::setSampleIntervalMs(int64_t ms) {
    m_sampleIntervalMs = std::max(int64_t(100), std::min(ms, int64_t(1000)));
    return *this;
}

OpButtonMetricConfigBuilder& OpButtonMetricConfigBuilder::setBufferSize(int v) { m_bufferSize = v; return *this; }
OpButtonMetricConfigBuilder& OpButtonMetricConfigBuilder::setBatchSize(int v) { m_batchSize = v; return *this; }
OpButtonMetricConfigBuilder& OpButtonMetricConfigBuilder::addCollector(OpButtonMetricCollector* c) { m_collectors.push_back(c); return *this; }

bool OpButtonMetricConfigBuilder::isEnabled() const { return m_enabled; }
int64_t OpButtonMetricConfigBuilder::sampleIntervalMs() const { return m_sampleIntervalMs; }
int OpButtonMetricConfigBuilder::bufferSize() const { return m_bufferSize; }
int OpButtonMetricConfigBuilder::batchSize() const { return m_batchSize; }
const OpButtonVector<OpButtonMetricCollector*>& OpButtonMetricConfigBuilder::collectors() const { return m_collectors; }

} // namespace OpButton
