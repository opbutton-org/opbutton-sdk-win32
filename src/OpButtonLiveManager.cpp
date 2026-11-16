#include "OpButton/OpButtonLiveManager.h"
#ifndef OPBUTTON_NO_LIVEKIT
#include "OpButtonLiveKitPublisher.h"
#endif
#include "OpButton/OpButtonException.h"
#include "OpButton/OpButtonHttpClient.h"
#include "OpButton/OpButtonLog.h"
#include "OpButton/OpButtonScreenCaptureProvider.h"

#include <algorithm>
#include <chrono>

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    static int clampInt( int _value, int _lo, int _hi )
    {
        return std::min( std::max( _value, _lo ), _hi );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::LiveSettings OpButtonLiveManager::LiveSettings::low()
    {
        return { "low", 540, 2, 500 };
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::LiveSettings OpButtonLiveManager::LiveSettings::balanced()
    {
        return { "balanced", 720, 3, 400 };
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::LiveSettings OpButtonLiveManager::LiveSettings::high()
    {
        return { "high", 1080, 4, 300 };
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::LiveSettings OpButtonLiveManager::LiveSettings::fromPayload( const OpButtonJson & _payload )
    {
        if( _payload.isNull() == true )
        {
            return low();
        }

        OpButtonString preset = _payload.optString( "preset", "low" );
        LiveSettings base;
        if( preset == "balanced" )
        {
            base = balanced();
        }
        else if( preset == "high" )
        {
            base = high();
        }
        else
        {
            base = low();
            preset = "low";
        }

        return { preset, clampInt( _payload.optInt( "max_dimension", base.maxDimension ), 240, 1440 ), clampInt( _payload.optInt( "fps", base.framesPerSecond ), 1, 6 ),
                 static_cast<int64_t>( clampInt( _payload.optInt( "action_poll_interval_ms", static_cast<int>( base.actionPollIntervalMs ) ), 200, 2000 ) ) };
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::OpButtonLiveManager( OpButtonHttpClient & _httpClient )
        : m_httpClient( _httpClient )
        , m_captureProvider( nullptr )
#ifndef OPBUTTON_NO_LIVEKIT
        , m_publisher( new OpButtonLiveKitPublisher() )
#endif
        , m_liveSettings( LiveSettings::low() )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::~OpButtonLiveManager()
    {
        this->shutdown();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveManager::setScreenCaptureProvider( OpButtonScreenCaptureProvider * _provider )
    {
        std::lock_guard<OpButtonMutex> lock( m_mutex );
        m_captureProvider = _provider;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveManager::setOnLiveModeChanged( OpButtonFunction<void( bool, int64_t )> _callback )
    {
        std::lock_guard<OpButtonMutex> lock( m_mutex );
        m_onLiveModeChanged = _callback;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonLiveManager::isLive() const
    {
        return m_isLive.load();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLiveManager::handleStart( const OpButtonString & _sessionToken, const OpButtonJson & _payload )
    {
#ifdef OPBUTTON_NO_LIVEKIT
        (void)_sessionToken;
        (void)_payload;
        OpButtonLog::w( "Live streaming is not supported on this platform (x86)" );
        return buildAck( false, "live_not_supported_on_x86" );
#else
        bool wasLive = m_isLive.load();
        this->stopLiveRuntime_();

        if( wasLive == true )
        {
            std::lock_guard<OpButtonMutex> lock( m_mutex );
            if( m_onLiveModeChanged != nullptr )
            {
                m_onLiveModeChanged( false, 0 );
            }
        }

        {
            std::lock_guard<OpButtonMutex> lock( m_mutex );
            if( m_captureProvider == nullptr )
            {
                OpButtonLog::e( "Live start failed: no screen capture provider set" );
                return buildAck( false, "no_screen_capture_provider" );
            }
        }

        try
        {
            LiveSettings requested = LiveSettings::fromPayload( _payload );
            PublisherSession session = this->fetchPublisherSession_( _sessionToken, requested );
            LiveSettings settings = session.settings;

            {
                std::lock_guard<OpButtonMutex> lock( m_mutex );
                m_isLive.store( true );
                m_liveSettings = settings;
                m_captureProvider->onLiveSettingsChanged( settings.maxDimension, settings.framesPerSecond );
            }

            // Connect to LiveKit room and publish video track
            m_publisher->start( session.serverUrl, session.participantToken, settings.maxDimension, settings.maxDimension );

            // Start frame capture thread
            m_captureRunning.store( true );
            m_captureThread = OpButtonThread( [this]
                                              { this->frameCaptureLoop_(); } );

            {
                std::lock_guard<OpButtonMutex> lock( m_mutex );
                if( m_onLiveModeChanged != nullptr )
                {
                    m_onLiveModeChanged( true, settings.actionPollIntervalMs );
                }
            }

            OpButtonLog::i( "Live started (preset=%s, %dx%d@%dfps)", settings.presetName.c_str(), settings.maxDimension, settings.maxDimension, settings.framesPerSecond );

            OpButtonJson ack = buildAck( true, "live_started" );
            ack.put( "preset", OpButtonJson( settings.presetName ) );
            ack.put( "max_dimension", OpButtonJson( settings.maxDimension ) );
            ack.put( "fps", OpButtonJson( settings.framesPerSecond ) );
            ack.put( "room_name", OpButtonJson( session.roomName ) );
            ack.put( "video_track_name", OpButtonJson( session.videoTrackName ) );
            return ack;
        }
        catch( const OpButtonException & e )
        {
            this->stopLiveRuntime_();
            OpButtonLog::e( "Failed to start live: %s", e.what() );
            return buildAck( false, OpButtonString( e.what() ) );
        }
        catch( ... )
        {
            this->stopLiveRuntime_();
            OpButtonLog::e( "Failed to start live: unknown error" );
            return buildAck( false, "unknown_error" );
        }
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLiveManager::handleUpdate( const OpButtonString & _sessionToken, const OpButtonJson & _payload )
    {
#ifdef OPBUTTON_NO_LIVEKIT
        (void)_sessionToken;
        (void)_payload;
        OpButtonLog::w( "Live streaming is not supported on this platform (x86)" );
        return buildAck( false, "live_not_supported_on_x86" );
#else
        if( m_isLive.load() == false )
        {
            return buildAck( false, "Live is not active" );
        }

        try
        {
            LiveSettings requested = LiveSettings::fromPayload( _payload );
            PublisherSession session = this->fetchPublisherSession_( _sessionToken, requested );
            LiveSettings settings = session.settings;

            {
                std::lock_guard<OpButtonMutex> lock( m_mutex );
                m_liveSettings = settings;
                if( m_captureProvider != nullptr )
                {
                    m_captureProvider->onLiveSettingsChanged( settings.maxDimension, settings.framesPerSecond );
                }
            }

            // Wake up capture thread so it picks up new FPS
            m_captureSleepCv.notify_all();

            {
                std::lock_guard<OpButtonMutex> lock( m_mutex );
                if( m_onLiveModeChanged != nullptr )
                {
                    m_onLiveModeChanged( true, settings.actionPollIntervalMs );
                }
            }

            OpButtonLog::i( "Live updated (preset=%s, %dx%d@%dfps)", settings.presetName.c_str(), settings.maxDimension, settings.maxDimension, settings.framesPerSecond );

            OpButtonJson ack = buildAck( true, "live_updated" );
            ack.put( "preset", OpButtonJson( settings.presetName ) );
            ack.put( "max_dimension", OpButtonJson( settings.maxDimension ) );
            ack.put( "fps", OpButtonJson( settings.framesPerSecond ) );
            ack.put( "room_name", OpButtonJson( session.roomName ) );
            ack.put( "video_track_name", OpButtonJson( session.videoTrackName ) );
            return ack;
        }
        catch( const OpButtonException & e )
        {
            OpButtonLog::e( "Failed to update live quality: %s", e.what() );
            return buildAck( false, OpButtonString( e.what() ) );
        }
        catch( ... )
        {
            OpButtonLog::e( "Failed to update live quality: unknown error" );
            return buildAck( false, "unknown_error" );
        }
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLiveManager::handleStop()
    {
#ifdef OPBUTTON_NO_LIVEKIT
        return buildAck( true, "live_not_supported_on_x86" );
#else
        bool wasLive = m_isLive.load();
        this->stopLiveRuntime_();

        {
            std::lock_guard<OpButtonMutex> lock( m_mutex );
            if( m_onLiveModeChanged != nullptr )
            {
                m_onLiveModeChanged( false, 0 );
            }
        }

        if( wasLive == true )
        {
            OpButtonLog::i( "Live stopped" );
        }

        return buildAck( true, wasLive ? "live_stopped" : "live_already_stopped" );
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLiveManager::handleTap( const OpButtonJson & _payload )
    {
        (void)_payload;
        if( m_isLive.load() == false )
        {
            return buildAck( false, "Live is not active" );
        }

        // Win32 does not support touch dispatch
        return buildAck( false, "Touch dispatch is not available on Windows" );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLiveManager::handleLongPress( const OpButtonJson & _payload )
    {
        (void)_payload;
        if( m_isLive.load() == false )
        {
            return buildAck( false, "Live is not active" );
        }

        // Win32 does not support touch dispatch
        return buildAck( false, "Touch dispatch is not available on Windows" );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveManager::onSessionClosed()
    {
        this->stopLiveRuntime_();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveManager::shutdown()
    {
        this->stopLiveRuntime_();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveManager::stopLiveRuntime_()
    {
        m_isLive.store( false );
        m_captureRunning.store( false );
        m_captureSleepCv.notify_all();

        if( m_captureThread.joinable() == true )
        {
            m_captureThread.join();
        }

#ifndef OPBUTTON_NO_LIVEKIT
        m_publisher->stop();
#endif

        std::lock_guard<OpButtonMutex> lock( m_mutex );
        m_liveSettings = LiveSettings::low();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::PublisherSession OpButtonLiveManager::fetchPublisherSession_( const OpButtonString & _sessionToken, const LiveSettings & _requested )
    {

        OpButtonJson response = m_httpClient.getLivePublisherState( _sessionToken );

        if( response.optBool( "configured", false ) == false )
        {
            OpButtonString msg = response.optString( "status_message", "LiveKit is not configured on the server" );
            throw OpButtonException( msg );
        }

        if( response.optBool( "requested", false ) == false )
        {
            OpButtonString msg = response.optString( "status_message", "Live request is no longer active" );
            throw OpButtonException( msg );
        }

        OpButtonString serverUrl = response.optString( "server_url" );
        OpButtonString participantToken = response.optString( "participant_token" );
        OpButtonString videoTrackName = response.optString( "video_track_name" );

        if( serverUrl.empty() || participantToken.empty() || videoTrackName.empty() )
        {
            throw OpButtonException( "Server returned incomplete live credentials" );
        }

        OpButtonString presetName = response.optString( "preset" );
        if( presetName.empty() == true )
        {
            presetName = _requested.presetName;
        }
        int maxDimension = clampInt( response.optInt( "max_dimension", _requested.maxDimension ), 240, 1440 );
        int fps = clampInt( response.optInt( "fps", _requested.framesPerSecond ), 1, 6 );
        int64_t actionPollIntervalMs = static_cast<int64_t>( clampInt( response.optInt( "action_poll_interval_ms", static_cast<int>( _requested.actionPollIntervalMs ) ), 200, 2000 ) );

        return PublisherSession{ serverUrl,
                                 participantToken,
                                 response.optString( "room_name" ),
                                 response.optString( "participant_identity" ),
                                 videoTrackName,
                                 LiveSettings{ presetName, maxDimension, fps, actionPollIntervalMs } };
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveManager::frameCaptureLoop_()
    {
        OpButtonLog::d( "Frame capture thread started" );

        while( m_captureRunning.load() == true )
        {
            int fps;
            OpButtonScreenCaptureProvider * provider;

            {
                std::lock_guard<OpButtonMutex> lock( m_mutex );
                fps = m_liveSettings.framesPerSecond;
                provider = m_captureProvider;
            }

            if( provider == nullptr )
            {
                std::unique_lock<OpButtonMutex> lock( m_captureSleepMutex );
                m_captureSleepCv.wait_for( lock, std::chrono::milliseconds( 500 ), [this]
                                           { return !m_captureRunning.load(); } );
                continue;
            }

            OpButtonCapturedFrame frame = {};
            bool captured = false;

            try
            {
                captured = provider->captureFrame( frame );
            }
            catch( ... )
            {
                OpButtonLog::e( "Screen capture provider threw exception" );
            }

            if( captured && frame.data && frame.dataSize > 0 )
            {
                try
                {
#ifndef OPBUTTON_NO_LIVEKIT
                    m_publisher->pushFrame( frame.data, frame.width, frame.height );
#endif
                }
                catch( const OpButtonException & e )
                {
                    OpButtonLog::w( "Failed to push live frame: %s", e.what() );
                }
            }

            int64_t sleepMs = fps > 0 ? ( 1000 / fps ) : 500;
            std::unique_lock<OpButtonMutex> lock( m_captureSleepMutex );
            m_captureSleepCv.wait_for( lock, std::chrono::milliseconds( sleepMs ), [this]
                                       { return !m_captureRunning.load(); } );
        }

        OpButtonLog::d( "Frame capture thread stopped" );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLiveManager::buildAck( bool _ok, const OpButtonString & _status )
    {
        OpButtonJson ack = OpButtonJson::object();
        ack.put( "ok", OpButtonJson( _ok ) );
        ack.put( "status", OpButtonJson( _status ) );
        return ack;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::LiveSettings::LiveSettings()
        : presetName()
        , maxDimension()
        , framesPerSecond()
        , actionPollIntervalMs()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::LiveSettings::~LiveSettings()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::LiveSettings::LiveSettings( const OpButtonString & _presetName, int _maxDimension, int _framesPerSecond, int64_t _actionPollIntervalMs )
        : presetName( _presetName )
        , maxDimension( _maxDimension )
        , framesPerSecond( _framesPerSecond )
        , actionPollIntervalMs( _actionPollIntervalMs )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::PublisherSession::PublisherSession()
        : serverUrl()
        , participantToken()
        , roomName()
        , participantIdentity()
        , videoTrackName()
        , settings()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::PublisherSession::~PublisherSession()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveManager::PublisherSession::PublisherSession( const OpButtonString & _serverUrl, const OpButtonString & _participantToken, const OpButtonString & _roomName, const OpButtonString & _participantIdentity, const OpButtonString & _videoTrackName, const LiveSettings & _settings )
        : serverUrl( _serverUrl )
        , participantToken( _participantToken )
        , roomName( _roomName )
        , participantIdentity( _participantIdentity )
        , videoTrackName( _videoTrackName )
        , settings( _settings )
    {
    }
} // namespace OpButton
