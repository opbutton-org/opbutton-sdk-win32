#pragma once

#include "OpButton/OpButtonTypes.h"
#include <cstdint>

namespace OpButton
{
    class OpButtonLiveKitPublisher
        : public OpButtonRefCounted
    {
    public:
        OpButtonLiveKitPublisher();
        ~OpButtonLiveKitPublisher() override;

    public:
        void start( const OpButtonString & _serverUrl, const OpButtonString & _participantToken, int _width, int _height );
        void pushFrame( const uint8_t * _argbData, int _width, int _height );
        void updateResolution( int _width, int _height );
        void stop();
        bool isConnected() const;

    private:
        class State;

    private:
        OpButtonIntrusivePtr<State> m_state;
    };
} // namespace OpButton
