#include "OpButton/OpButtonSdk.h"
#include "OpButton/OpButtonHttpClient.h"
#include "OpButton/OpButtonLiveManager.h"
#include "OpButton/OpButtonDefaultMetricCollector.h"
#include "OpButton/OpButtonLog.h"
#include "OpButton/OpButtonAction.h"
#include "OpButton/OpButtonException.h"

#include "OpButtonWin32.h"

#include <chrono>
#include <algorithm>

namespace OpButton {

// ══════════════════════════════════════════════════════
//  Internal instance — holds all mutable state
// ══════════════════════════════════════════════════════

class OpButtonInstance {
public:
    OpButtonInstance(const OpButtonConfig& config);
    ~OpButtonInstance();

    void startConnection();
    void stopConnection();
    void doShutdown();
    void bufferLog(const OpButtonLogEntry& entry);
    OpButtonJson resolveLogAttributes();
    void dispatchQueuedActions();

    OpButtonConfig config;
    OpButtonHttpClient httpClient;
    OpButtonLiveManager liveManager;
    OpButtonString sessionToken;
    OpButtonMutex sessionTokenMutex;
    OpButtonAtomic<OpButtonSessionStatus> status{OpButtonSessionStatus::PL_DISCONNECTED};
    OpButtonUI ui;
    OpButtonMetrics metrics;
    OpButtonDefaultMetricCollector defaultMetricCollector;

    OpButtonVector<OpButtonActionListener*> actionListeners;
    OpButtonVector<OpButtonSessionListener*> sessionListeners;
    OpButtonMutex listenerMutex;

private:
    OpButtonAtomic<bool> m_running{false};
    OpButtonMutex m_sleepMutex;
    OpButtonConditionVariable m_sleepCv;

    OpButtonThread m_connectionThread;
    OpButtonThread m_actionPollThread;
    OpButtonThread m_metricSampleThread;

    int64_t m_currentActionPollIntervalMs;

    OpButtonVector<OpButtonLogEntry> m_logBuffer;
    OpButtonMutex m_logBufferMutex;
    bool m_logOverflowWarned = false;

    OpButtonMap<OpButtonString, OpButtonJson> m_sessionAttributeCache;

    OpButtonVector<OpButtonAction> m_actionQueue;
    OpButtonMutex m_actionQueueMutex;

    void interruptibleSleep(int64_t ms);

    // Connection
    void connectAndWaitApproval();
    void doConnectAndWaitApproval();
    void onApproved(const OpButtonString& token);
    void onRejected();

    // Action polling
    void actionPollLoop(const OpButtonString& token);
    void doPollActions(const OpButtonString& token);

    // Metric sampling
    void metricSampleLoop();

    // Session attributes
    OpButtonMap<OpButtonString, OpButtonJson> resolveAllSessionAttributes();
    OpButtonMap<OpButtonString, OpButtonJson> resolveChangedSessionAttributes();

    // Log buffering
    OpButtonVector<OpButtonLogEntry> drainLogChunk();
    void requeueLogs(const OpButtonVector<OpButtonLogEntry>& chunk);
    void flushLogs(const OpButtonString& token);
    void flushMetrics(const OpButtonString& token);

    // Reconnection
    void attemptReconnect(const OpButtonString& token);
    void resetAndRestart();

    // Helpers
    void setStatus(OpButtonSessionStatus s) { status.store(s); }

    void notifyConnecting();
    void notifyWaitingApproval(const OpButtonString& reqId);
    void notifySessionStarted(const OpButtonString& token);
    void notifySessionClosed();
    void notifyRejected();
    void notifyAuthFailed();
    void notifyError(const OpButtonException& e);
};

// ══════════════════════════════════════════════════════
//  Static singleton state
// ══════════════════════════════════════════════════════

static OpButtonInstance* s_instance = nullptr;
static OpButtonMutex s_initMutex;

static OpButtonInstance* requireInstance() {
    if (!s_instance) {
        OpButtonLog::e("OpButtonSdk::initialize() must be called first");
    }
    return s_instance;
}

// ══════════════════════════════════════════════════════
//  OpButtonSdk static methods
// ══════════════════════════════════════════════════════

void OpButtonSdk::initialize(const OpButtonConfig& config) {
    std::lock_guard<OpButtonMutex> lock(s_initMutex);
    if (s_instance) {
        OpButtonLog::w("OpButtonSdk::initialize() called more than once, ignoring");
        return;
    }

    s_instance = new OpButtonInstance(config);
    OpButtonLog::i("OpButton SDK initialized (server: %s)", config.baseUrl().c_str());

    if (config.sessionListener()) {
        s_instance->sessionListeners.push_back(config.sessionListener());
    }
    if (config.actionListener()) {
        s_instance->actionListeners.push_back(config.actionListener());
    }

    auto& mc = config.metricConfig();
    if (mc.isEnabled()) {
        s_instance->metrics.addCollector(&s_instance->defaultMetricCollector);
        for (auto* collector : mc.collectors()) {
            s_instance->metrics.addCollector(collector);
        }
        OpButtonLog::i("Built-in metrics enabled (sample interval: %lldms)", static_cast<long long>(mc.sampleIntervalMs()));
    }

    if (config.autoConnect()) {
        s_instance->startConnection();
    }
}

bool OpButtonSdk::isInitialized() {
    return s_instance != nullptr;
}

OpButtonSessionStatus OpButtonSdk::status() {
    auto* p = s_instance;
    return p ? p->status.load() : OpButtonSessionStatus::PL_DISCONNECTED;
}

void OpButtonSdk::addActionListener(OpButtonActionListener* listener) {
    auto* p = requireInstance();
    if (!p) return;
    std::lock_guard<OpButtonMutex> lock(p->listenerMutex);
    p->actionListeners.push_back(listener);
}

void OpButtonSdk::removeActionListener(OpButtonActionListener* listener) {
    auto* p = s_instance;
    if (!p) return;
    std::lock_guard<OpButtonMutex> lock(p->listenerMutex);
    auto& v = p->actionListeners;
    v.erase(std::remove(v.begin(), v.end(), listener), v.end());
}

void OpButtonSdk::addSessionListener(OpButtonSessionListener* listener) {
    auto* p = requireInstance();
    if (!p) return;
    std::lock_guard<OpButtonMutex> lock(p->listenerMutex);
    p->sessionListeners.push_back(listener);
}

void OpButtonSdk::removeSessionListener(OpButtonSessionListener* listener) {
    auto* p = s_instance;
    if (!p) return;
    std::lock_guard<OpButtonMutex> lock(p->listenerMutex);
    auto& v = p->sessionListeners;
    v.erase(std::remove(v.begin(), v.end(), listener), v.end());
}

OpButtonUI& OpButtonSdk::getUI() {
    return requireInstance()->ui;
}

OpButtonMetrics& OpButtonSdk::getMetrics() {
    return requireInstance()->metrics;
}

void OpButtonSdk::setScreenCaptureProvider(OpButtonScreenCaptureProvider* provider) {
    auto* p = requireInstance();
    if (p) p->liveManager.setScreenCaptureProvider(provider);
}

void OpButtonSdk::connect() {
    auto* p = requireInstance();
    if (p) p->startConnection();
}

void OpButtonSdk::disconnect() {
    auto* p = s_instance;
    if (p) p->stopConnection();
}

void OpButtonSdk::update() {
    auto* p = s_instance;
    if (!p) return;
    p->ui.pollValues();
    p->dispatchQueuedActions();
}

void OpButtonSdk::log(OpButtonLogLevel level, const OpButtonString& message) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(OpButtonLogEntry(level, message, OpButtonString(), OpButtonString(), {}, p->resolveLogAttributes()));
}

void OpButtonSdk::log(OpButtonLogLevel level, const OpButtonString& message,
                    const OpButtonString& category, const OpButtonString& thread) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(OpButtonLogEntry(level, message, category, thread, {}, p->resolveLogAttributes()));
}

void OpButtonSdk::log(OpButtonLogLevel level, const OpButtonString& message,
                    const OpButtonMap<OpButtonString, OpButtonJson>& metadata) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(OpButtonLogEntry(level, message, OpButtonString(), OpButtonString(), metadata, p->resolveLogAttributes()));
}

void OpButtonSdk::log(OpButtonLogLevel level, const OpButtonString& message,
                    const OpButtonString& category, const OpButtonString& thread,
                    const OpButtonMap<OpButtonString, OpButtonJson>& metadata) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(OpButtonLogEntry(level, message, category, thread, metadata, p->resolveLogAttributes()));
}

void OpButtonSdk::log(const OpButtonLogEntry& entry) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(entry);
}

static void bufferStructuredLog(OpButtonInstance* p, const OpButtonString& kind,
                                const OpButtonString& message, const OpButtonString& category,
                                const OpButtonMap<OpButtonString, OpButtonJson>& metadata,
                                int64_t clientTimestampMs) {
    if (!p || !p->config.logConfig().isEnabled()) return;

    OpButtonString resolvedCategory;
    if (category.empty()) {
        resolvedCategory = kind;
    } else if (category == kind || category.substr(0, kind.size() + 1) == kind + OpButtonString("_")) {
        resolvedCategory = category;
    } else {
        resolvedCategory = kind + OpButtonString("_") + category;
    }

    OpButtonMap<OpButtonString, OpButtonJson> merged = metadata;
    merged["opbutton_kind"] = OpButtonJson(kind);

    p->bufferLog(OpButtonLogEntry(
        OpButtonLogLevel::PL_INFO, message, resolvedCategory, OpButtonString(),
        merged, p->resolveLogAttributes(), clientTimestampMs));
}

static int64_t nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void OpButtonSdk::event(const OpButtonString& message) {
    bufferStructuredLog(s_instance, "event", message, OpButtonString(), {}, nowMs());
}

void OpButtonSdk::event(const OpButtonString& message, const OpButtonMap<OpButtonString, OpButtonJson>& metadata) {
    bufferStructuredLog(s_instance, "event", message, OpButtonString(), metadata, nowMs());
}

void OpButtonSdk::event(const OpButtonString& message, const OpButtonString& category,
                      const OpButtonMap<OpButtonString, OpButtonJson>& metadata) {
    bufferStructuredLog(s_instance, "event", message, category, metadata, nowMs());
}

void OpButtonSdk::revenue(const OpButtonString& message) {
    bufferStructuredLog(s_instance, "revenue", message, OpButtonString(), {}, nowMs());
}

void OpButtonSdk::revenue(const OpButtonString& message, const OpButtonMap<OpButtonString, OpButtonJson>& metadata) {
    bufferStructuredLog(s_instance, "revenue", message, OpButtonString(), metadata, nowMs());
}

void OpButtonSdk::revenue(const OpButtonString& message, const OpButtonString& category,
                        const OpButtonMap<OpButtonString, OpButtonJson>& metadata) {
    bufferStructuredLog(s_instance, "revenue", message, category, metadata, nowMs());
}

void OpButtonSdk::changeScreen(const OpButtonString& screenType, const OpButtonString& screenName) {
    OpButtonMap<OpButtonString, OpButtonJson> metadata;
    metadata["opbutton_command"] = OpButtonJson("change_screen");
    metadata["opbutton_slice_type"] = OpButtonJson("screen");
    metadata["opbutton_slice_name"] = OpButtonJson(screenName);
    metadata["screen_type"] = OpButtonJson(screenType);
    metadata["screen_name"] = OpButtonJson(screenName);
    bufferStructuredLog(s_instance, "event", "change_screen", "change_screen", metadata, nowMs());
}

void OpButtonSdk::acknowledgeAction(const OpButtonString& actionId, const OpButtonJson& ackPayload) {
    auto* p = s_instance;
    if (!p) return;
    OpButtonString token;
    {
        std::lock_guard<OpButtonMutex> lock(p->sessionTokenMutex);
        token = p->sessionToken;
    }
    if (token.empty()) return;
    try {
        p->httpClient.acknowledgeAction(token, actionId, ackPayload);
    } catch (const OpButtonException& e) {
        OpButtonLog::e("Failed to acknowledge action: %s", e.what());
    }
}

void OpButtonSdk::shutdown() {
    std::lock_guard<OpButtonMutex> lock(s_initMutex);
    auto* p = s_instance;
    s_instance = nullptr;
    if (p) {
        p->doShutdown();
        delete p;
    }
}

// ══════════════════════════════════════════════════════
//  OpButtonInstance implementation
// ══════════════════════════════════════════════════════

OpButtonInstance::OpButtonInstance(const OpButtonConfig& config)
    : config(config)
    , httpClient(config.baseUrl(), config.apiToken())
    , liveManager(httpClient)
    , m_currentActionPollIntervalMs(config.actionPollIntervalMs()) {
    OpButtonLog::setLevel(config.logConfig().logLevel());
    OpButtonLog::setLoggerListener(config.loggerListener());

    liveManager.setOnLiveModeChanged([this](bool isLive, int64_t pollIntervalMs) {
        if (isLive && pollIntervalMs > 0) {
            m_currentActionPollIntervalMs = pollIntervalMs;
        } else {
            m_currentActionPollIntervalMs = this->config.actionPollIntervalMs();
        }
    });

    auto& mc = config.metricConfig();
    if (mc.isEnabled()) {
        metrics.sampleIntervalMs = mc.sampleIntervalMs();
        metrics.bufferSize = mc.bufferSize();
        metrics.batchSize = mc.batchSize();
    }
}

OpButtonInstance::~OpButtonInstance() {
    doShutdown();
}

void OpButtonInstance::startConnection() {
    if (m_running.exchange(true)) {
        OpButtonLog::w("Already connecting/connected");
        return;
    }

    m_connectionThread = OpButtonThread([this] { connectAndWaitApproval(); });
}

void OpButtonInstance::stopConnection() {
    if (!m_running.exchange(false)) return;

    liveManager.onSessionClosed();

    // Wake up sleeping threads
    m_sleepCv.notify_all();

    // Wait for threads to finish
    if (m_connectionThread.joinable()) m_connectionThread.join();
    if (m_actionPollThread.joinable()) m_actionPollThread.join();
    if (m_metricSampleThread.joinable()) m_metricSampleThread.join();

    OpButtonString token;
    {
        std::lock_guard<OpButtonMutex> lock(sessionTokenMutex);
        token = sessionToken;
        sessionToken.clear();
    }

    if (!token.empty()) {
        try {
            flushLogs(token);
            flushMetrics(token);
            httpClient.closeSession(token);
        } catch (...) {}
        setStatus(OpButtonSessionStatus::PL_CLOSED);
        notifySessionClosed();
    } else {
        setStatus(OpButtonSessionStatus::PL_DISCONNECTED);
    }
}

void OpButtonInstance::doShutdown() {
    OpButtonLog::i("Shutting down OpButton SDK");
    liveManager.shutdown();
    stopConnection();
    httpClient.shutdown();
    {
        std::lock_guard<OpButtonMutex> lock(listenerMutex);
        actionListeners.clear();
        sessionListeners.clear();
    }
    {
        std::lock_guard<OpButtonMutex> lock(m_logBufferMutex);
        m_logBuffer.clear();
    }
    metrics.clear();
}

void OpButtonInstance::bufferLog(const OpButtonLogEntry& entry) {
    std::lock_guard<OpButtonMutex> lock(m_logBufferMutex);
    if (static_cast<int>(m_logBuffer.size()) >= config.logConfig().bufferSize()) {
        m_logBuffer.erase(m_logBuffer.begin());
        if (!m_logOverflowWarned) {
            m_logOverflowWarned = true;
            OpButtonLog::w("Log buffer overflow (%d), dropping oldest entries", config.logConfig().bufferSize());
        }
    }
    m_logBuffer.push_back(entry);
}

OpButtonJson OpButtonInstance::resolveLogAttributes() {
    const auto& builder = config.logConfig().attributes();
    const auto& staticAttrs = builder.staticAttributes();
    const auto& dynamicAttrs = builder.dynamicAttributes();

    if (staticAttrs.empty() && dynamicAttrs.empty()) return OpButtonJson();

    OpButtonJson attributes = OpButtonJson::object();
    for (const auto& kv : staticAttrs) {
        attributes.put(kv.first, OpButtonJson(kv.second));
    }
    for (const auto& kv : dynamicAttrs) {
        try {
            OpButtonString val = kv.second->getValue();
            attributes.put(kv.first, OpButtonJson(val));
        } catch (...) {}
    }

    return attributes.objectSize() == 0 ? OpButtonJson() : attributes;
}

void OpButtonInstance::dispatchQueuedActions() {
    OpButtonVector<OpButtonAction> actions;
    {
        std::lock_guard<OpButtonMutex> lock(m_actionQueueMutex);
        actions.swap(m_actionQueue);
    }

    OpButtonString token;
    {
        std::lock_guard<OpButtonMutex> lock(sessionTokenMutex);
        token = sessionToken;
    }

    for (const auto& action : actions) {
        // Intercept LIVE_* actions internally
        OpButtonActionType type = action.actionType();
        if (type == OpButtonActionType::LIVE_START || type == OpButtonActionType::LIVE_UPDATE ||
            type == OpButtonActionType::LIVE_STOP || type == OpButtonActionType::LIVE_TAP ||
            type == OpButtonActionType::LIVE_LONG_PRESS) {
            OpButtonJson ack;
            switch (type) {
                case OpButtonActionType::LIVE_START:
                    ack = liveManager.handleStart(token, action.payload());
                    break;
                case OpButtonActionType::LIVE_UPDATE:
                    ack = liveManager.handleUpdate(token, action.payload());
                    break;
                case OpButtonActionType::LIVE_STOP:
                    ack = liveManager.handleStop();
                    break;
                case OpButtonActionType::LIVE_TAP:
                    ack = liveManager.handleTap(action.payload());
                    break;
                case OpButtonActionType::LIVE_LONG_PRESS:
                    ack = liveManager.handleLongPress(action.payload());
                    break;
                default:
                    break;
            }

            if (!ack.isNull() && !action.id().empty() && !token.empty()) {
                try {
                    httpClient.acknowledgeAction(token, action.id(), ack);
                } catch (const OpButtonException& e) {
                    OpButtonLog::e("Failed to acknowledge live action: %s", e.what());
                }
            }
            continue;
        }

        bool handled = false;
        try {
            handled = ui.dispatchAction(action);
        } catch (...) {
            OpButtonLog::e("Widget callback threw exception");
        }

        if (!handled) {
            std::lock_guard<OpButtonMutex> lock(listenerMutex);
            for (auto* listener : actionListeners) {
                try {
                    listener->onOpButtonActionReceived(action);
                } catch (...) {
                    OpButtonLog::e("Action listener threw exception");
                }
            }
        }
    }
}

void OpButtonInstance::interruptibleSleep(int64_t ms) {
    std::unique_lock<OpButtonMutex> lock(m_sleepMutex);
    m_sleepCv.wait_for(lock, std::chrono::milliseconds(ms), [this] { return !m_running.load(); });
}

// ── Connection logic ──

void OpButtonInstance::connectAndWaitApproval() {
    int64_t retryDelayMs = 2000;
    const int64_t maxRetryDelayMs = 30000;
    int retryCount = 0;

    while (m_running.load()) {
        try {
            doConnectAndWaitApproval();
            return;
        } catch (const OpButtonException& e) {
            if (e.isUnauthorized()) {
                OpButtonLog::e("Authentication failed: %s", e.what());
                setStatus(OpButtonSessionStatus::PL_AUTH_FAILED);
                m_running.store(false);
                notifyAuthFailed();
                return;
            }
            if (!e.isNetworkError()) {
                OpButtonLog::e("Server error, stopping connection: %s", e.what());
                setStatus(OpButtonSessionStatus::PL_ERROR);
                m_running.store(false);
                notifyError(e);
                return;
            }

            retryCount++;
            OpButtonLog::w("Connection attempt %d failed (network): %s, retrying in %lldms",
                        retryCount, e.what(), static_cast<long long>(retryDelayMs));
            setStatus(OpButtonSessionStatus::PL_CONNECTING);
            interruptibleSleep(retryDelayMs);
            retryDelayMs = std::min(retryDelayMs * 2, maxRetryDelayMs);
        }
    }
}

void OpButtonInstance::doConnectAndWaitApproval() {
    setStatus(OpButtonSessionStatus::PL_CONNECTING);
    notifyConnecting();

    OpButtonString deviceId = config.deviceId();
    OpButtonString deviceName = config.deviceName();

    if (deviceId.empty()) {
        char compName[256] = {};
        DWORD size = sizeof(compName);
        GetComputerNameA(compName, &size);
        deviceId = compName;
    }
    if (deviceName.empty()) {
        char compName[256] = {};
        DWORD size = sizeof(compName);
        GetComputerNameA(compName, &size);
        deviceName = OpButtonString(compName) + OpButtonString(" (Windows)");
    }

    auto resp = httpClient.connect(deviceId, deviceName, resolveAllSessionAttributes());

    OpButtonLog::i("Connect request sent, request_id=%s, status=%s",
                resp.requestId().c_str(), resp.status().c_str());

    if (resp.isApproved() && !resp.sessionToken().empty()) {
        onApproved(resp.sessionToken());
        return;
    }

    if (resp.isRejected()) {
        onRejected();
        return;
    }

    setStatus(OpButtonSessionStatus::PL_WAITING_APPROVAL);
    notifyWaitingApproval(resp.requestId());

    while (m_running.load()) {
        interruptibleSleep(config.pollIntervalMs());
        if (!m_running.load()) return;

        auto pollResp = httpClient.pollStatus(resp.requestId());

        if (pollResp.isApproved() && !pollResp.sessionToken().empty()) {
            onApproved(pollResp.sessionToken());
            return;
        }

        if (pollResp.isRejected()) {
            onRejected();
            return;
        }
    }
}

void OpButtonInstance::onApproved(const OpButtonString& token) {
    {
        std::lock_guard<OpButtonMutex> lock(sessionTokenMutex);
        sessionToken = token;
    }
    setStatus(OpButtonSessionStatus::PL_ACTIVE);
    OpButtonLog::i("Session approved and active");
    notifySessionStarted(token);

    // Submit initial UI if available
    if (ui.hasTabs()) {
        OpButtonJson snapshot = ui.toJson();
        try {
            httpClient.submitPanel(token, snapshot);
            ui.markSent();
            OpButtonLog::d("Initial UI submitted (revision=%d)", ui.getRevision());
        } catch (const OpButtonException& e) {
            OpButtonLog::e("Failed to submit initial UI: %s", e.what());
            notifyError(e);
        }
    }

    // Start action polling thread
    m_actionPollThread = OpButtonThread([this, token] { actionPollLoop(token); });

    // Start metric sampling thread
    if (config.metricConfig().isEnabled()) {
        m_metricSampleThread = OpButtonThread([this] { metricSampleLoop(); });
    }
}

void OpButtonInstance::onRejected() {
    OpButtonLog::w("Connection request rejected");
    setStatus(OpButtonSessionStatus::PL_REJECTED);
    m_running.store(false);
    notifyRejected();
}

// ── Action polling ──

void OpButtonInstance::actionPollLoop(const OpButtonString& token) {
    while (m_running.load()) {
        doPollActions(token);
        interruptibleSleep(m_currentActionPollIntervalMs);
    }
}

void OpButtonInstance::doPollActions(const OpButtonString& token) {
    try {
        auto changedAttrs = resolveChangedSessionAttributes();
        auto logChunk = drainLogChunk();
        auto metricChunk = metrics.drain();

        // Snapshot and submit UI if changed
        if (ui.hasUnsent()) {
            OpButtonJson uiSnapshot = ui.toJson();
            try {
                httpClient.submitPanel(token, uiSnapshot);
                ui.markSent();
                OpButtonLog::d("UI submitted (revision=%d)", ui.getRevision());
            } catch (const OpButtonException& e) {
                OpButtonLog::e("Failed to submit UI: %s", e.what());
                notifyError(e);
            }
        }

        const OpButtonMap<OpButtonString, OpButtonJson>* changedPtr = changedAttrs.empty() ? nullptr : &changedAttrs;
        OpButtonJson json = httpClient.pollActions(token, changedPtr, logChunk, metricChunk);
        OpButtonJson actionsArr = json.optArray("actions");

        if (actionsArr.length() > 0) {
            std::lock_guard<OpButtonMutex> lock(m_actionQueueMutex);
            for (int i = 0; i < actionsArr.length(); ++i) {
                OpButtonJson actionJson = actionsArr.at(i);
                if (!actionJson.isNull()) {
                    m_actionQueue.push_back(OpButtonAction::fromJson(actionJson));
                }
            }
        }

        if (!logChunk.empty()) {
            m_logOverflowWarned = false;
        }
    } catch (const OpButtonException& e) {
        if (e.isNetworkError()) {
            OpButtonLog::w("Network error during action poll, will retry");
        } else {
            OpButtonLog::w("Action poll failed with server error, attempting reconnect");
            attemptReconnect(token);
        }
    }
}

// ── Metric sampling ──

void OpButtonInstance::metricSampleLoop() {
    while (m_running.load()) {
        metrics.sample();
        interruptibleSleep(metrics.sampleIntervalMs);
    }
}

// ── Session attributes ──

OpButtonMap<OpButtonString, OpButtonJson> OpButtonInstance::resolveAllSessionAttributes() {
    const auto& builder = config.sessionAttributes();
    OpButtonMap<OpButtonString, OpButtonJson> merged = builder.staticAttributes();

    for (const auto& kv : builder.dynamicAttributes()) {
        try {
            OpButtonString val = kv.second->getValue();
            OpButtonJson resolved(val);
            merged[kv.first] = resolved;
            m_sessionAttributeCache[kv.first] = resolved;
        } catch (...) {
            OpButtonLog::e("Session attribute provider failed: %s", kv.first.c_str());
        }
    }

    return merged;
}

OpButtonMap<OpButtonString, OpButtonJson> OpButtonInstance::resolveChangedSessionAttributes() {
    const auto& dynamicAttrs = config.sessionAttributes().dynamicAttributes();
    if (dynamicAttrs.empty()) return {};

    OpButtonMap<OpButtonString, OpButtonJson> changed;

    for (const auto& kv : dynamicAttrs) {
        try {
            OpButtonString val = kv.second->getValue();
            OpButtonJson resolved(val);
            auto cached = m_sessionAttributeCache.find(kv.first);
            if (cached == m_sessionAttributeCache.end() || cached->second.asString() != val) {
                m_sessionAttributeCache[kv.first] = resolved;
                changed[kv.first] = resolved;
            }
        } catch (...) {
            OpButtonLog::e("Session attribute provider failed: %s", kv.first.c_str());
        }
    }

    return changed;
}

// ── Log buffering ──

OpButtonVector<OpButtonLogEntry> OpButtonInstance::drainLogChunk() {
    std::lock_guard<OpButtonMutex> lock(m_logBufferMutex);
    if (m_logBuffer.empty()) return {};

    int count = std::min(static_cast<int>(m_logBuffer.size()), config.logConfig().batchSize());
    OpButtonVector<OpButtonLogEntry> chunk(m_logBuffer.begin(), m_logBuffer.begin() + count);
    m_logBuffer.erase(m_logBuffer.begin(), m_logBuffer.begin() + count);
    return chunk;
}

void OpButtonInstance::requeueLogs(const OpButtonVector<OpButtonLogEntry>& chunk) {
    if (chunk.empty()) return;
    std::lock_guard<OpButtonMutex> lock(m_logBufferMutex);
    m_logBuffer.insert(m_logBuffer.begin(), chunk.begin(), chunk.end());
    while (static_cast<int>(m_logBuffer.size()) > config.logConfig().bufferSize()) {
        m_logBuffer.pop_back();
    }
}

void OpButtonInstance::flushLogs(const OpButtonString& token) {
    auto chunk = drainLogChunk();
    if (chunk.empty()) return;
    try {
        httpClient.sendLogs(token, chunk);
    } catch (const OpButtonException&) {
        requeueLogs(chunk);
    }
}

void OpButtonInstance::flushMetrics(const OpButtonString& token) {
    auto chunk = metrics.drain();
    if (chunk.empty()) return;
    try {
        httpClient.sendMetrics(token, chunk);
    } catch (const OpButtonException&) {
        metrics.requeue(chunk);
    }
}

// ── Reconnection ──

void OpButtonInstance::attemptReconnect(const OpButtonString& token) {
    setStatus(OpButtonSessionStatus::PL_CONNECTING);

    int64_t retryDelay = 2000;
    const int64_t maxDelay = 30000;

    while (m_running.load()) {
        interruptibleSleep(retryDelay);
        if (!m_running.load()) return;

        try {
            httpClient.pollActions(token, nullptr, {}, {});
            OpButtonLog::i("Reconnect successful, session still active");
            setStatus(OpButtonSessionStatus::PL_ACTIVE);
            return;
        } catch (const OpButtonException& e) {
            if (e.isNetworkError()) {
                OpButtonLog::w("Reconnect: no network, retrying in %lldms", static_cast<long long>(retryDelay));
                retryDelay = std::min(retryDelay * 2, maxDelay);
            } else {
                OpButtonLog::w("Session lost (server error), starting fresh connection");
                resetAndRestart();
                return;
            }
        }
    }
}

void OpButtonInstance::resetAndRestart() {
    m_running.store(false);
    {
        std::lock_guard<OpButtonMutex> lock(sessionTokenMutex);
        sessionToken.clear();
    }
    setStatus(OpButtonSessionStatus::PL_DISCONNECTED);
    notifySessionClosed();
    startConnection();
}

// ── Listener notifications ──

void OpButtonInstance::notifyConnecting() {
    std::lock_guard<OpButtonMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onOpButtonSessionConnecting();
}

void OpButtonInstance::notifyWaitingApproval(const OpButtonString& reqId) {
    std::lock_guard<OpButtonMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onOpButtonSessionWaitingApproval(reqId);
}

void OpButtonInstance::notifySessionStarted(const OpButtonString& token) {
    std::lock_guard<OpButtonMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onOpButtonSessionStarted(token);
}

void OpButtonInstance::notifySessionClosed() {
    std::lock_guard<OpButtonMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onOpButtonSessionClosed();
}

void OpButtonInstance::notifyRejected() {
    std::lock_guard<OpButtonMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onOpButtonSessionRejected();
}

void OpButtonInstance::notifyAuthFailed() {
    std::lock_guard<OpButtonMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onOpButtonSessionAuthFailed();
}

void OpButtonInstance::notifyError(const OpButtonException& e) {
    std::lock_guard<OpButtonMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onOpButtonSessionError(e);
}

} // namespace OpButton
