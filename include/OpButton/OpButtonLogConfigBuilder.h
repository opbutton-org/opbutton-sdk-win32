#pragma once

#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonLogAttributeBuilder.h"

namespace OpButton {

class OpButtonLogConfigBuilder {
public:
    OpButtonLogConfigBuilder& setEnabled(bool v);
    OpButtonLogConfigBuilder& setLogLevel(OpButtonLogLevel v);
    OpButtonLogConfigBuilder& setBatchSize(int v);
    OpButtonLogConfigBuilder& setBufferSize(int v);
    OpButtonLogConfigBuilder& setAttributes(const OpButtonLogAttributeBuilder& v);

    bool isEnabled() const;
    OpButtonLogLevel logLevel() const;
    int batchSize() const;
    int bufferSize() const;
    const OpButtonLogAttributeBuilder& attributes() const;

private:
    bool m_enabled = true;
    OpButtonLogLevel m_logLevel = OpButtonLogLevel::PL_INFO;
    int m_batchSize = 100;
    int m_bufferSize = 1000;
    OpButtonLogAttributeBuilder m_attributes;
};

} // namespace OpButton
