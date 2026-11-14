#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonLogLevel.h"
#include <cstdint>

namespace OpButton {

class OpButtonLogEntry {
public:
    OpButtonLogEntry(OpButtonLogLevel level, const OpButtonString& message,
                  const OpButtonString& category = OpButtonString(),
                  const OpButtonString& thread = OpButtonString(),
                  const OpButtonMap<OpButtonString, OpButtonJson>& metadata = {},
                  const OpButtonJson& attributes = OpButtonJson(),
                  int64_t clientTimestampMs = 0);

    OpButtonLogEntry(const OpButtonString& level, const OpButtonString& message,
                  const OpButtonString& category = OpButtonString(),
                  const OpButtonString& thread = OpButtonString(),
                  const OpButtonMap<OpButtonString, OpButtonJson>& metadata = {},
                  const OpButtonJson& attributes = OpButtonJson(),
                  int64_t clientTimestampMs = 0);

    OpButtonJson toJson() const;

    static OpButtonLogEntry debug(const OpButtonString& msg);
    static OpButtonLogEntry info(const OpButtonString& msg);
    static OpButtonLogEntry warning(const OpButtonString& msg);
    static OpButtonLogEntry error(const OpButtonString& msg);
    static OpButtonLogEntry critical(const OpButtonString& msg);

private:
    OpButtonString m_level;
    OpButtonString m_message;
    OpButtonString m_category;
    OpButtonString m_thread;
    OpButtonMap<OpButtonString, OpButtonJson> m_metadata;
    OpButtonJson m_attributes;
    OpButtonString m_clientTimestamp;

    static OpButtonString formatTimestamp(int64_t ms);
    static int64_t currentTimeMs();
};

} // namespace OpButton
