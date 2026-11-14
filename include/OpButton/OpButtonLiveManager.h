#pragma once

#include "OpButton/OpButtonTypes.h"
#include "OpButton/OpButtonJson.h"

namespace OpButton {

class OpButtonHttpClient;
class OpButtonScreenCaptureProvider;
#ifndef OPBUTTON_NO_LIVEKIT
class OpButtonLiveKitPublisher;
#endif

class OpButtonLiveManager {
public:
    OpButtonLiveManager(OpButtonHttpClient& httpClient);
    ~OpButtonLiveManager();

    void setScreenCaptureProvider(OpButtonScreenCaptureProvider* provider);
    void setOnLiveModeChanged(OpButtonFunction<void(bool, int64_t)> callback);

    OpButtonJson handleStart(const OpButtonString& sessionToken, const OpButtonJson& payload);
    OpButtonJson handleUpdate(const OpButtonString& sessionToken, const OpButtonJson& payload);
    OpButtonJson handleStop();
    OpButtonJson handleTap(const OpButtonJson& payload);
    OpButtonJson handleLongPress(const OpButtonJson& payload);

    bool isLive() const;
    void onSessionClosed();
    void shutdown();

private:
    struct LiveSettings {
        OpButtonString presetName;
        int maxDimension;
        int framesPerSecond;
        int64_t actionPollIntervalMs;

        static LiveSettings low();
        static LiveSettings balanced();
        static LiveSettings high();
        static LiveSettings fromPayload(const OpButtonJson& payload);
    };

    struct PublisherSession {
        OpButtonString serverUrl;
        OpButtonString participantToken;
        OpButtonString roomName;
        OpButtonString participantIdentity;
        OpButtonString videoTrackName;
        LiveSettings settings;
    };

    void stopLiveRuntime();
    PublisherSession fetchPublisherSession(const OpButtonString& sessionToken, const LiveSettings& requested);
    void frameCaptureLoop();

    static OpButtonJson buildAck(bool ok, const OpButtonString& status);

    OpButtonHttpClient& m_httpClient;
    OpButtonScreenCaptureProvider* m_captureProvider;
    OpButtonFunction<void(bool, int64_t)> m_onLiveModeChanged;

#ifndef OPBUTTON_NO_LIVEKIT
    OpButtonUniquePtr<OpButtonLiveKitPublisher> m_publisher;
#endif

    mutable OpButtonMutex m_mutex;
    OpButtonAtomic<bool> m_isLive{false};
    LiveSettings m_liveSettings;

    OpButtonAtomic<bool> m_captureRunning{false};
    OpButtonThread m_captureThread;
    OpButtonMutex m_captureSleepMutex;
    OpButtonConditionVariable m_captureSleepCv;
};

} // namespace OpButton
