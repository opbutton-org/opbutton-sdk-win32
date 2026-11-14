#include "OpButtonLiveKitPublisher.h"
#include "OpButton/OpButtonLog.h"
#include "OpButton/OpButtonException.h"

namespace OpButton {

static bool s_livekitInitialized = false;

static void ensureLiveKitInitialized() {
    if (!s_livekitInitialized) {
        livekit::initialize(livekit::LogLevel::Warn);
        s_livekitInitialized = true;
    }
}

OpButtonLiveKitPublisher::OpButtonLiveKitPublisher()
    : m_connected(false) {
}

OpButtonLiveKitPublisher::~OpButtonLiveKitPublisher() {
    stop();
}

void OpButtonLiveKitPublisher::start(const OpButtonString& serverUrl,
                                   const OpButtonString& participantToken,
                                   int width, int height) {
    stop();

    ensureLiveKitInitialized();

    std::string url(serverUrl.begin(), serverUrl.end());
    std::string token(participantToken.begin(), participantToken.end());

    m_room = std::make_unique<livekit::Room>();

    livekit::RoomOptions opts;
    opts.auto_subscribe = false;
    opts.dynacast = false;

    bool connected = m_room->Connect(url, token, opts);
    if (!connected) {
        m_room.reset();
        throw OpButtonException("Failed to connect to LiveKit room");
    }

    m_videoSource = std::make_shared<livekit::VideoSource>(width, height);

    auto* participant = m_room->localParticipant();
    if (!participant) {
        m_room.reset();
        m_videoSource.reset();
        throw OpButtonException("No local participant after LiveKit connect");
    }

    m_videoTrack = participant->publishVideoTrack(
        "opbutton-app-screen",
        m_videoSource,
        livekit::TrackSource::SOURCE_SCREENSHARE);

    if (!m_videoTrack) {
        m_room.reset();
        m_videoSource.reset();
        throw OpButtonException("Failed to publish video track to LiveKit");
    }

    m_connected = true;
    OpButtonLog::i("LiveKit publisher started (%dx%d)", width, height);
}

void OpButtonLiveKitPublisher::pushFrame(const uint8_t* argbData, int width, int height) {
    if (!m_connected || !m_videoSource) {
        return;
    }

    size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    std::vector<uint8_t> frameData(argbData, argbData + dataSize);

    livekit::VideoFrame frame(width, height, livekit::VideoBufferType::ARGB, std::move(frameData));
    m_videoSource->captureFrame(frame);
}

void OpButtonLiveKitPublisher::updateResolution(int width, int height) {
    (void)width;
    (void)height;
}

void OpButtonLiveKitPublisher::stop() {
    if (m_videoTrack) {
        if (m_room && m_room->localParticipant()) {
            std::string sid(m_videoTrack->sid());
            m_room->localParticipant()->unpublishTrack(sid);
        }
        m_videoTrack.reset();
    }

    m_videoSource.reset();
    m_room.reset();
    m_connected = false;
}

bool OpButtonLiveKitPublisher::isConnected() const {
    return m_connected;
}

} // namespace OpButton
