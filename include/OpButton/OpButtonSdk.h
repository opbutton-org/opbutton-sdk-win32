#pragma once

#include "OpButton/OpButtonConfig.h"
#include "OpButton/OpButtonSessionStatus.h"
#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonLogEntry.h"
#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonMetrics.h"
#include "OpButton/OpButtonActionListener.h"
#include "OpButton/OpButtonSessionListener.h"
#include "OpButton/OpButtonScreenCaptureProvider.h"

namespace OpButton {

class OpButtonSdk {
public:
    static constexpr const char* VERSION = "1.0.38";

    static void initialize(const OpButtonConfig& config);
    static bool isInitialized();
    static OpButtonSessionStatus status();

    static void addActionListener(OpButtonActionListener* listener);
    static void removeActionListener(OpButtonActionListener* listener);
    static void addSessionListener(OpButtonSessionListener* listener);
    static void removeSessionListener(OpButtonSessionListener* listener);

    static OpButtonUI& getUI();
    static OpButtonMetrics& getMetrics();

    static void setScreenCaptureProvider(OpButtonScreenCaptureProvider* provider);

    static void connect();
    static void disconnect();

    // Call from your main loop to poll value providers and dispatch queued callbacks
    static void update();

    static void log(OpButtonLogLevel level, const OpButtonString& message);
    static void log(OpButtonLogLevel level, const OpButtonString& message,
                    const OpButtonString& category, const OpButtonString& thread = OpButtonString());
    static void log(OpButtonLogLevel level, const OpButtonString& message,
                    const OpButtonMap<OpButtonString, OpButtonJson>& metadata);
    static void log(OpButtonLogLevel level, const OpButtonString& message,
                    const OpButtonString& category, const OpButtonString& thread,
                    const OpButtonMap<OpButtonString, OpButtonJson>& metadata);
    static void log(const OpButtonLogEntry& entry);

    static void event(const OpButtonString& message);
    static void event(const OpButtonString& message, const OpButtonMap<OpButtonString, OpButtonJson>& metadata);
    static void event(const OpButtonString& message, const OpButtonString& category,
                      const OpButtonMap<OpButtonString, OpButtonJson>& metadata);

    static void revenue(const OpButtonString& message);
    static void revenue(const OpButtonString& message, const OpButtonMap<OpButtonString, OpButtonJson>& metadata);
    static void revenue(const OpButtonString& message, const OpButtonString& category,
                        const OpButtonMap<OpButtonString, OpButtonJson>& metadata);

    static void changeScreen(const OpButtonString& screenType, const OpButtonString& screenName);

    static void acknowledgeAction(const OpButtonString& actionId, const OpButtonJson& ackPayload = OpButtonJson());

    static void shutdown();

private:
    OpButtonSdk() = delete;
};

} // namespace OpButton
