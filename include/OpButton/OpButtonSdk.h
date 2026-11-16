#pragma once

#include "OpButton/OpButtonActionListener.h"
#include "OpButton/OpButtonConfig.h"
#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonLogEntry.h"
#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonMetrics.h"
#include "OpButton/OpButtonScreenCaptureProvider.h"
#include "OpButton/OpButtonSessionListener.h"
#include "OpButton/OpButtonSessionStatus.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton
{
    class OpButtonSdk
    {
    public:
        ~OpButtonSdk();

    public:
        static constexpr const char * VERSION = "1.0.44";

    public:
        static void initialize( const OpButtonConfig & _config );
        static bool isInitialized();
        static OpButtonSessionStatus status();

        static void addActionListener( OpButtonActionListener * _listener );
        static void removeActionListener( OpButtonActionListener * _listener );
        static void addSessionListener( OpButtonSessionListener * _listener );
        static void removeSessionListener( OpButtonSessionListener * _listener );

        static OpButtonUI & getUI();
        static OpButtonMetrics & getMetrics();

        static void setScreenCaptureProvider( OpButtonScreenCaptureProvider * _provider );

        static void connect();
        static void disconnect();

        // Call from your main loop to poll value providers and dispatch queued
        // callbacks
        static void update();

        static void log( OpButtonLogLevel _level, const OpButtonString & _message );
        static void log( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonString & _category, const OpButtonString & _thread = OpButtonString() );
        static void log( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata );
        static void log( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonString & _category, const OpButtonString & _thread, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata );
        static void log( const OpButtonLogEntry & _entry );

        static void event( const OpButtonString & _message );
        static void event( const OpButtonString & _message, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata );
        static void event( const OpButtonString & _message, const OpButtonString & _category, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata );

        static void revenue( const OpButtonString & _message );
        static void revenue( const OpButtonString & _message, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata );
        static void revenue( const OpButtonString & _message, const OpButtonString & _category, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata );

        static void changeScreen( const OpButtonString & _screenType, const OpButtonString & _screenName );

        static void acknowledgeAction( const OpButtonString & _actionId, const OpButtonJson & _ackPayload = OpButtonJson() );

        static void shutdown();

    private:
        OpButtonSdk();
    };

} // namespace OpButton
