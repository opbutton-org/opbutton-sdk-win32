#include "OpButton/OpButtonLogConfigBuilder.h"

namespace OpButton {

OpButtonLogConfigBuilder& OpButtonLogConfigBuilder::setEnabled(bool v) { m_enabled = v; return *this; }
OpButtonLogConfigBuilder& OpButtonLogConfigBuilder::setLogLevel(OpButtonLogLevel v) { m_logLevel = v; return *this; }
OpButtonLogConfigBuilder& OpButtonLogConfigBuilder::setBatchSize(int v) { m_batchSize = v; return *this; }
OpButtonLogConfigBuilder& OpButtonLogConfigBuilder::setBufferSize(int v) { m_bufferSize = v; return *this; }
OpButtonLogConfigBuilder& OpButtonLogConfigBuilder::setAttributes(const OpButtonLogAttributeBuilder& v) { m_attributes = v; return *this; }

bool OpButtonLogConfigBuilder::isEnabled() const { return m_enabled; }
OpButtonLogLevel OpButtonLogConfigBuilder::logLevel() const { return m_logLevel; }
int OpButtonLogConfigBuilder::batchSize() const { return m_batchSize; }
int OpButtonLogConfigBuilder::bufferSize() const { return m_bufferSize; }
const OpButtonLogAttributeBuilder& OpButtonLogConfigBuilder::attributes() const { return m_attributes; }

} // namespace OpButton
