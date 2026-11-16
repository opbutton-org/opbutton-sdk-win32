#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonHttpClient;
    class OpButtonScreenCaptureProvider;
#ifndef OPBUTTON_NO_LIVEKIT
    class OpButtonLiveKitPublisher;
#endif

    class OpButtonLiveManager
    {
    public:
        OpButtonLiveManager( OpButtonHttpClient & _httpClient );
        ~OpButtonLiveManager();

    public:
        void setScreenCaptureProvider( OpButtonScreenCaptureProvider * _provider );
        void setOnLiveModeChanged( OpButtonFunction<void( bool, int64_t )> _callback );

        OpButtonJson handleStart( const OpButtonString & _sessionToken, const OpButtonJson & _payload );
        OpButtonJson handleUpdate( const OpButtonString & _sessionToken, const OpButtonJson & _payload );
        OpButtonJson handleStop();
        OpButtonJson handleTap( const OpButtonJson & _payload );
        OpButtonJson handleLongPress( const OpButtonJson & _payload );

        bool isLive() const;
        void onSessionClosed();
        void shutdown();

    private:
        struct LiveSettings
        {
        public:
            LiveSettings();
            LiveSettings( const OpButtonString & _presetName, int _maxDimension, int _framesPerSecond, int64_t _actionPollIntervalMs );
            ~LiveSettings();

        public:
            OpButtonString presetName;
            int maxDimension;
            int framesPerSecond;
            int64_t actionPollIntervalMs;

        public:
            static LiveSettings low();
            static LiveSettings balanced();
            static LiveSettings high();
            static LiveSettings fromPayload( const OpButtonJson & _payload );
        };

        struct PublisherSession
        {
        public:
            PublisherSession();
            PublisherSession( const OpButtonString & _serverUrl, const OpButtonString & _participantToken, const OpButtonString & _roomName, const OpButtonString & _participantIdentity, const OpButtonString & _videoTrackName, const LiveSettings & _settings );
            ~PublisherSession();

        public:
            OpButtonString serverUrl;
            OpButtonString participantToken;
            OpButtonString roomName;
            OpButtonString participantIdentity;
            OpButtonString videoTrackName;
            LiveSettings settings;
        };

    private:
        void stopLiveRuntime_();
        PublisherSession fetchPublisherSession_( const OpButtonString & _sessionToken, const LiveSettings & _requested );
        void frameCaptureLoop_();

        static OpButtonJson buildAck( bool _ok, const OpButtonString & _status );

    private:
        OpButtonHttpClient & m_httpClient;
        OpButtonScreenCaptureProvider * m_captureProvider;
        OpButtonFunction<void( bool, int64_t )> m_onLiveModeChanged;

#ifndef OPBUTTON_NO_LIVEKIT
        OpButtonIntrusivePtr<OpButtonLiveKitPublisher> m_publisher;
#endif

        mutable OpButtonMutex m_mutex;
        OpButtonAtomic<bool> m_isLive{ false };
        LiveSettings m_liveSettings;

        OpButtonAtomic<bool> m_captureRunning{ false };
        OpButtonThread m_captureThread;
        OpButtonMutex m_captureSleepMutex;
        OpButtonConditionVariable m_captureSleepCv;
    };

} // namespace OpButton
