#include "OpButton/OpButtonLogEntry.h"

#include <chrono>
#include <ctime>

namespace OpButton {

static OpButtonString formatTimestampImpl(int64_t ms) {
    time_t seconds = static_cast<time_t>(ms / 1000);
    struct tm utcBuf;
#ifdef _WIN32
    gmtime_s(&utcBuf, &seconds);
#else
    gmtime_r(&seconds, &utcBuf);
#endif
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utcBuf);
    return buf;
}

static int64_t currentTimeMsImpl() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

OpButtonLogEntry::OpButtonLogEntry(OpButtonLogLevel level, const OpButtonString& message,
                             const OpButtonString& category, const OpButtonString& thread,
                             const OpButtonMap<OpButtonString, OpButtonJson>& metadata,
                             const OpButtonJson& attributes, int64_t clientTimestampMs)
    : m_level(opbuttonLogLevelValue(level))
    , m_message(message)
    , m_category(category)
    , m_thread(thread)
    , m_metadata(metadata)
    , m_attributes(attributes)
    , m_clientTimestamp(formatTimestampImpl(clientTimestampMs > 0 ? clientTimestampMs : currentTimeMsImpl())) {
}

OpButtonLogEntry::OpButtonLogEntry(const OpButtonString& level, const OpButtonString& message,
                             const OpButtonString& category, const OpButtonString& thread,
                             const OpButtonMap<OpButtonString, OpButtonJson>& metadata,
                             const OpButtonJson& attributes, int64_t clientTimestampMs)
    : m_level(level)
    , m_message(message)
    , m_category(category)
    , m_thread(thread)
    , m_metadata(metadata)
    , m_attributes(attributes)
    , m_clientTimestamp(formatTimestampImpl(clientTimestampMs > 0 ? clientTimestampMs : currentTimeMsImpl())) {
}

OpButtonJson OpButtonLogEntry::toJson() const {
    OpButtonJson json = OpButtonJson::object();
    json.put("level", OpButtonJson(m_level));
    json.put("message", OpButtonJson(m_message));
    json.put("client_timestamp", OpButtonJson(m_clientTimestamp));
    if (!m_category.empty()) {
        json.put("category", OpButtonJson(m_category));
    }
    if (!m_thread.empty()) {
        json.put("thread", OpButtonJson(m_thread));
    }
    if (!m_metadata.empty()) {
        OpButtonJson metaJson = OpButtonJson::object();
        for (const auto& kv : m_metadata) {
            metaJson.put(kv.first, kv.second);
        }
        json.put("metadata", metaJson);
    }
    if (!m_attributes.isNull()) {
        json.put("attributes", m_attributes);
    }
    return json;
}

OpButtonString OpButtonLogEntry::formatTimestamp(int64_t ms) { return formatTimestampImpl(ms); }
int64_t OpButtonLogEntry::currentTimeMs() { return currentTimeMsImpl(); }

OpButtonLogEntry OpButtonLogEntry::debug(const OpButtonString& msg) { return OpButtonLogEntry(OpButtonLogLevel::PL_DEBUG, msg); }
OpButtonLogEntry OpButtonLogEntry::info(const OpButtonString& msg) { return OpButtonLogEntry(OpButtonLogLevel::PL_INFO, msg); }
OpButtonLogEntry OpButtonLogEntry::warning(const OpButtonString& msg) { return OpButtonLogEntry(OpButtonLogLevel::PL_WARNING, msg); }
OpButtonLogEntry OpButtonLogEntry::error(const OpButtonString& msg) { return OpButtonLogEntry(OpButtonLogLevel::PL_ERROR, msg); }
OpButtonLogEntry OpButtonLogEntry::critical(const OpButtonString& msg) { return OpButtonLogEntry(OpButtonLogLevel::PL_CRITICAL, msg); }

} // namespace OpButton
