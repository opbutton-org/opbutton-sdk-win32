#include "OpButtonLiveKitPublisher.h"
#include "OpButton/OpButtonException.h"
#include "OpButton/OpButtonLog.h"

#include <livekit/livekit.h>
#include <memory>
#include <mutex>

namespace OpButton
{
    namespace Detail
    {
        static std::once_flag s_liveKitInitialize;
    }

    // LiveKit's publication API requires shared_ptr for its source and track.
    // These foreign ownership types remain inside the adapter.
    class OpButtonLiveKitPublisher::State
        : public OpButtonRefCounted
    {
    public:
        State();
        ~State() override;

    public:
        livekit::Room m_room;
        std::shared_ptr<livekit::VideoSource> m_videoSource;
        std::shared_ptr<livekit::LocalVideoTrack> m_videoTrack;
    };

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveKitPublisher::State::State()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveKitPublisher::State::~State()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveKitPublisher::OpButtonLiveKitPublisher()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLiveKitPublisher::~OpButtonLiveKitPublisher()
    {
        this->stop();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveKitPublisher::start( const OpButtonString & _serverUrl, const OpButtonString & _participantToken, int _width, int _height )
    {
        this->stop();
        std::call_once( Detail::s_liveKitInitialize, []()
                        { livekit::initialize( livekit::LogLevel::Warn ); } );

        std::string url( _serverUrl.begin(), _serverUrl.end() );
        std::string token( _participantToken.begin(), _participantToken.end() );
        OpButtonIntrusivePtr<State> state( new State() );
        livekit::RoomOptions options;
        options.auto_subscribe = false;
        options.dynacast = false;

        bool connected = state->m_room.Connect( url, token, options );
        if( connected == false )
        {
            throw OpButtonException( "Failed to connect to LiveKit room" );
        }

        livekit::LocalParticipant * participant = state->m_room.localParticipant();
        if( participant == nullptr )
        {
            throw OpButtonException( "No local participant after LiveKit connect" );
        }

        state->m_videoSource = std::allocate_shared<livekit::VideoSource>( OpButtonStlAllocator<livekit::VideoSource>(), _width, _height );
        state->m_videoTrack = participant->publishVideoTrack( "opbutton-app-screen", state->m_videoSource, livekit::TrackSource::SOURCE_SCREENSHARE );
        if( state->m_videoTrack == nullptr )
        {
            throw OpButtonException( "Failed to publish video track to LiveKit" );
        }

        m_state = std::move( state );
        OpButtonLog::i( "LiveKit publisher started (%dx%d)", _width, _height );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveKitPublisher::pushFrame( const uint8_t * _argbData, int _width, int _height )
    {
        if( m_state == nullptr )
        {
            return;
        }
        size_t dataSize = static_cast<size_t>( _width ) * static_cast<size_t>( _height ) * 4;
        std::vector<uint8_t> frameData( _argbData, _argbData + dataSize );
        livekit::VideoFrame frame( _width, _height, livekit::VideoBufferType::ARGB, std::move( frameData ) );
        m_state->m_videoSource->captureFrame( frame );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveKitPublisher::updateResolution( int _width, int _height )
    {
        (void)_width;
        (void)_height;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonLiveKitPublisher::stop()
    {
        OpButtonIntrusivePtr<State> state;
        state.swap( m_state );
        if( state == nullptr )
        {
            return;
        }
        // The Room destructor disconnects even if explicit unpublication fails.
        try
        {
            livekit::LocalParticipant * participant = state->m_room.localParticipant();
            if( participant == nullptr )
            {
                return;
            }
            if( state->m_videoTrack == nullptr )
            {
                return;
            }
            std::string sid( state->m_videoTrack->sid() );
            participant->unpublishTrack( sid );
        }
        catch( const std::exception & _exception )
        {
            OpButtonLog::e( "Failed to unpublish LiveKit video track: %s", _exception.what() );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonLiveKitPublisher::isConnected() const
    {
        return m_state != nullptr;
    }
} // namespace OpButton
