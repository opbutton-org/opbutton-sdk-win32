#pragma once

#include "OpButton/OpButtonTypes.h"

#include <livekit/livekit.h>

#include <memory>
#include <cstdint>

namespace OpButton {

class OpButtonLiveKitPublisher {
public:
    OpButtonLiveKitPublisher();
    ~OpButtonLiveKitPublisher();

    void start(const OpButtonString& serverUrl,
               const OpButtonString& participantToken,
               int width, int height);

    void pushFrame(const uint8_t* argbData, int width, int height);
    void updateResolution(int width, int height);
    void stop();

    bool isConnected() const;

private:
    std::unique_ptr<livekit::Room> m_room;
    std::shared_ptr<livekit::VideoSource> m_videoSource;
    std::shared_ptr<livekit::LocalVideoTrack> m_videoTrack;
    bool m_connected;
};

} // namespace OpButton
