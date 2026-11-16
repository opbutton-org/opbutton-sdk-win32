#include "OpButton/OpButtonSdk.h"
#include "OpButton/OpButtonAction.h"
#include "OpButton/OpButtonDefaultMetricCollector.h"
#include "OpButton/OpButtonException.h"
#include "OpButton/OpButtonHttpClient.h"
#include "OpButton/OpButtonLiveManager.h"
#include "OpButton/OpButtonLog.h"

#include "OpButtonWin32.h"

#include <algorithm>
#include <chrono>

namespace OpButton
{

    //  Internal instance — holds all mutable state

    class OpButtonInstance
        : public OpButtonRefCounted
    {
    public:
        OpButtonInstance( const OpButtonConfig & _config );
        ~OpButtonInstance();

    public:
        void startConnection();
        void stopConnection();
        void doShutdown();
        void bufferLog( const OpButtonLogEntry & _entry );
        OpButtonJson resolveLogAttributes();
        void dispatchQueuedActions();

    public:
        OpButtonConfig m_config;
        OpButtonHttpClient m_httpClient;
        OpButtonLiveManager m_liveManager;
        OpButtonString m_sessionToken;
        OpButtonMutex m_sessionTokenMutex;
        OpButtonAtomic<OpButtonSessionStatus> m_status{ OpButtonSessionStatus::PL_DISCONNECTED };
        OpButtonUI m_ui;
        OpButtonMetrics m_metrics;
        OpButtonDefaultMetricCollector m_defaultMetricCollector;

        OpButtonVector<OpButtonActionListener *> m_actionListeners;
        OpButtonVector<OpButtonSessionListener *> m_sessionListeners;
        OpButtonMutex m_listenerMutex;

    private:
        OpButtonAtomic<bool> m_running{ false };
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

    private:
        void interruptibleSleep_( int64_t _ms );

        // Connection
        void connectAndWaitApproval_();
        void doConnectAndWaitApproval_();
        void onApproved_( const OpButtonString & _token );
        void onRejected_();

        // Action polling
        void actionPollLoop_( const OpButtonString & _token );
        void doPollActions_( const OpButtonString & _token );

        // Metric sampling
        void metricSampleLoop_();

        // Session attributes
        OpButtonMap<OpButtonString, OpButtonJson> resolveAllSessionAttributes_();
        OpButtonMap<OpButtonString, OpButtonJson> resolveChangedSessionAttributes_();

        // Log buffering
        OpButtonVector<OpButtonLogEntry> drainLogChunk_();
        void requeueLogs_( const OpButtonVector<OpButtonLogEntry> & _chunk );
        void flushLogs_( const OpButtonString & _token );
        void flushMetrics_( const OpButtonString & _token );

        // Reconnection
        void attemptReconnect_( const OpButtonString & _token );
        void resetAndRestart_();

        // Helpers
        void setStatus_( OpButtonSessionStatus _s )
        {
            m_status.store( _s );
        }

        void notifyConnecting_();
        void notifyWaitingApproval_( const OpButtonString & _reqId );
        void notifySessionStarted_( const OpButtonString & _token );
        void notifySessionClosed_();
        void notifyRejected_();
        void notifyAuthFailed_();
        void notifyError_( const OpButtonException & _e );
    };

    //  Static singleton state

    static OpButtonInstance * s_instance = nullptr;
    static OpButtonMutex s_initMutex;

    //////////////////////////////////////////////////////////////////////////
    static OpButtonInstance * requireInstance()
    {
        if( s_instance == nullptr )
        {
            OpButtonLog::e( "OpButtonSdk::initialize() must be called first" );
        }
        return s_instance;
    }

    //  OpButtonSdk static methods

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::initialize( const OpButtonConfig & _config )
    {
        std::lock_guard<OpButtonMutex> lock( s_initMutex );
        if( s_instance != nullptr )
        {
            OpButtonLog::w( "OpButtonSdk::initialize() called more than once, ignoring" );
            return;
        }

        s_instance = new OpButtonInstance( _config );
        OpButtonLog::i( "OpButton SDK initialized (server: %s)", _config.baseUrl().c_str() );

        if( _config.sessionListener() != nullptr )
        {
            s_instance->m_sessionListeners.push_back( _config.sessionListener() );
        }
        if( _config.actionListener() != nullptr )
        {
            s_instance->m_actionListeners.push_back( _config.actionListener() );
        }

        const OpButtonMetricConfigBuilder & mc = _config.metricConfig();
        if( mc.isEnabled() == true )
        {
            s_instance->m_metrics.addCollector( &s_instance->m_defaultMetricCollector );
            for( auto * collector : mc.collectors() )
            {
                s_instance->m_metrics.addCollector( collector );
            }
            OpButtonLog::i( "Built-in metrics enabled (sample interval: %lldms)", static_cast<long long>( mc.sampleIntervalMs() ) );
        }

        if( _config.autoConnect() == true )
        {
            s_instance->startConnection();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonSdk::isInitialized()
    {
        return s_instance != nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionStatus OpButtonSdk::status()
    {
        OpButtonInstance * p = s_instance;
        return p ? p->m_status.load() : OpButtonSessionStatus::PL_DISCONNECTED;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::addActionListener( OpButtonActionListener * _listener )
    {
        OpButtonInstance * p = requireInstance();
        if( p == nullptr )
        {
            return;
        }
        std::lock_guard<OpButtonMutex> lock( p->m_listenerMutex );
        p->m_actionListeners.push_back( _listener );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::removeActionListener( OpButtonActionListener * _listener )
    {
        OpButtonInstance * p = s_instance;
        if( p == nullptr )
        {
            return;
        }
        std::lock_guard<OpButtonMutex> lock( p->m_listenerMutex );
        OpButtonVector<OpButtonActionListener *> & v = p->m_actionListeners;
        v.erase( std::remove( v.begin(), v.end(), _listener ), v.end() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::addSessionListener( OpButtonSessionListener * _listener )
    {
        OpButtonInstance * p = requireInstance();
        if( p == nullptr )
        {
            return;
        }
        std::lock_guard<OpButtonMutex> lock( p->m_listenerMutex );
        p->m_sessionListeners.push_back( _listener );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::removeSessionListener( OpButtonSessionListener * _listener )
    {
        OpButtonInstance * p = s_instance;
        if( p == nullptr )
        {
            return;
        }
        std::lock_guard<OpButtonMutex> lock( p->m_listenerMutex );
        OpButtonVector<OpButtonSessionListener *> & v = p->m_sessionListeners;
        v.erase( std::remove( v.begin(), v.end(), _listener ), v.end() );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonUI & OpButtonSdk::getUI()
    {
        return requireInstance()->m_ui;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMetrics & OpButtonSdk::getMetrics()
    {
        return requireInstance()->m_metrics;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::setScreenCaptureProvider( OpButtonScreenCaptureProvider * _provider )
    {
        OpButtonInstance * p = requireInstance();
        if( p != nullptr )
        {
            p->m_liveManager.setScreenCaptureProvider( _provider );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::connect()
    {
        OpButtonInstance * p = requireInstance();
        if( p != nullptr )
        {
            p->startConnection();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::disconnect()
    {
        OpButtonInstance * p = s_instance;
        if( p != nullptr )
        {
            p->stopConnection();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::update()
    {
        OpButtonInstance * p = s_instance;
        if( p == nullptr )
        {
            return;
        }
        p->m_ui.pollValues();
        p->dispatchQueuedActions();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::log( OpButtonLogLevel _level, const OpButtonString & _message )
    {
        OpButtonInstance * p = s_instance;
        if( !p || !p->m_config.logConfig().isEnabled() )
        {
            return;
        }
        p->bufferLog( OpButtonLogEntry( _level, _message, OpButtonString(), OpButtonString(), {}, p->resolveLogAttributes() ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::log( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonString & _category, const OpButtonString & _thread )
    {
        OpButtonInstance * p = s_instance;
        if( !p || !p->m_config.logConfig().isEnabled() )
        {
            return;
        }
        p->bufferLog( OpButtonLogEntry( _level, _message, _category, _thread, {}, p->resolveLogAttributes() ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::log( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata )
    {
        OpButtonInstance * p = s_instance;
        if( !p || !p->m_config.logConfig().isEnabled() )
        {
            return;
        }
        p->bufferLog( OpButtonLogEntry( _level, _message, OpButtonString(), OpButtonString(), _metadata, p->resolveLogAttributes() ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::log( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonString & _category, const OpButtonString & _thread, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata )
    {
        OpButtonInstance * p = s_instance;
        if( !p || !p->m_config.logConfig().isEnabled() )
        {
            return;
        }
        p->bufferLog( OpButtonLogEntry( _level, _message, _category, _thread, _metadata, p->resolveLogAttributes() ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::log( const OpButtonLogEntry & _entry )
    {
        OpButtonInstance * p = s_instance;
        if( !p || !p->m_config.logConfig().isEnabled() )
        {
            return;
        }
        p->bufferLog( _entry );
    }

    //////////////////////////////////////////////////////////////////////////
    static void bufferStructuredLog( OpButtonInstance * _p, const OpButtonString & _kind, const OpButtonString & _message, const OpButtonString & _category, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata, int64_t _clientTimestampMs )
    {
        if( !_p || !_p->m_config.logConfig().isEnabled() )
        {
            return;
        }

        OpButtonString resolvedCategory;
        if( _category.empty() == true )
        {
            resolvedCategory = _kind;
        }
        else if( _category == _kind || _category.substr( 0, _kind.size() + 1 ) == _kind + OpButtonString( "_" ) )
        {
            resolvedCategory = _category;
        }
        else
        {
            resolvedCategory = _kind + OpButtonString( "_" ) + _category;
        }

        OpButtonMap<OpButtonString, OpButtonJson> merged = _metadata;
        merged["opbutton_kind"] = OpButtonJson( _kind );

        _p->bufferLog( OpButtonLogEntry( OpButtonLogLevel::PL_INFO, _message, resolvedCategory, OpButtonString(), merged, _p->resolveLogAttributes(), _clientTimestampMs ) );
    }

    //////////////////////////////////////////////////////////////////////////
    static int64_t nowMs()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::event( const OpButtonString & _message )
    {
        bufferStructuredLog( s_instance, "event", _message, OpButtonString(), {}, nowMs() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::event( const OpButtonString & _message, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata )
    {
        bufferStructuredLog( s_instance, "event", _message, OpButtonString(), _metadata, nowMs() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::event( const OpButtonString & _message, const OpButtonString & _category, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata )
    {
        bufferStructuredLog( s_instance, "event", _message, _category, _metadata, nowMs() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::revenue( const OpButtonString & _message )
    {
        bufferStructuredLog( s_instance, "revenue", _message, OpButtonString(), {}, nowMs() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::revenue( const OpButtonString & _message, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata )
    {
        bufferStructuredLog( s_instance, "revenue", _message, OpButtonString(), _metadata, nowMs() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::revenue( const OpButtonString & _message, const OpButtonString & _category, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata )
    {
        bufferStructuredLog( s_instance, "revenue", _message, _category, _metadata, nowMs() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::changeScreen( const OpButtonString & _screenType, const OpButtonString & _screenName )
    {
        OpButtonMap<OpButtonString, OpButtonJson> metadata;
        metadata["opbutton_command"] = OpButtonJson( "change_screen" );
        metadata["opbutton_slice_type"] = OpButtonJson( "screen" );
        metadata["opbutton_slice_name"] = OpButtonJson( _screenName );
        metadata["screen_type"] = OpButtonJson( _screenType );
        metadata["screen_name"] = OpButtonJson( _screenName );
        bufferStructuredLog( s_instance, "event", "change_screen", "change_screen", metadata, nowMs() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::acknowledgeAction( const OpButtonString & _actionId, const OpButtonJson & _ackPayload )
    {
        OpButtonInstance * p = s_instance;
        if( p == nullptr )
        {
            return;
        }
        OpButtonString token;
        {
            std::lock_guard<OpButtonMutex> lock( p->m_sessionTokenMutex );
            token = p->m_sessionToken;
        }
        if( token.empty() == true )
        {
            return;
        }
        try
        {
            p->m_httpClient.acknowledgeAction( token, _actionId, _ackPayload );
        }
        catch( const OpButtonException & e )
        {
            OpButtonLog::e( "Failed to acknowledge action: %s", e.what() );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonSdk::shutdown()
    {
        std::lock_guard<OpButtonMutex> lock( s_initMutex );
        OpButtonInstance * p = s_instance;
        s_instance = nullptr;
        if( p != nullptr )
        {
            p->doShutdown();
            delete p;
        }
    }

    //  OpButtonInstance implementation

    //////////////////////////////////////////////////////////////////////////
    OpButtonInstance::OpButtonInstance( const OpButtonConfig & _config )
        : m_config( _config )
        , m_httpClient( _config.baseUrl(), _config.apiToken() )
        , m_liveManager( m_httpClient )
        , m_currentActionPollIntervalMs( _config.actionPollIntervalMs() )
    {
        OpButtonLog::setLevel( _config.logConfig().logLevel() );
        OpButtonLog::setLoggerListener( _config.loggerListener() );

        m_liveManager.setOnLiveModeChanged(
            [this]( bool _isLive, int64_t _pollIntervalMs )
            {
                if( _isLive && _pollIntervalMs > 0 )
                {
                    m_currentActionPollIntervalMs = _pollIntervalMs;
                }
                else
                {
                    m_currentActionPollIntervalMs = this->m_config.actionPollIntervalMs();
                }
            } );

        const OpButtonMetricConfigBuilder & mc = _config.metricConfig();
        if( mc.isEnabled() == true )
        {
            m_metrics.sampleIntervalMs = mc.sampleIntervalMs();
            m_metrics.bufferSize = mc.bufferSize();
            m_metrics.batchSize = mc.batchSize();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInstance::~OpButtonInstance()
    {
        this->doShutdown();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::startConnection()
    {
        if( m_running.exchange( true ) == true )
        {
            OpButtonLog::w( "Already connecting/connected" );
            return;
        }

        m_connectionThread = OpButtonThread( [this]
                                             { this->connectAndWaitApproval_(); } );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::stopConnection()
    {
        if( m_running.exchange( false ) == false )
        {
            return;
        }

        m_liveManager.onSessionClosed();

        // Wake up sleeping threads
        m_sleepCv.notify_all();

        // Wait for threads to finish
        if( m_connectionThread.joinable() == true )
        {
            m_connectionThread.join();
        }
        if( m_actionPollThread.joinable() == true )
        {
            m_actionPollThread.join();
        }
        if( m_metricSampleThread.joinable() == true )
        {
            m_metricSampleThread.join();
        }

        OpButtonString token;
        {
            std::lock_guard<OpButtonMutex> lock( m_sessionTokenMutex );
            token = m_sessionToken;
            m_sessionToken.clear();
        }

        if( token.empty() == false )
        {
            try
            {
                this->flushLogs_( token );
                this->flushMetrics_( token );
                m_httpClient.closeSession( token );
            }
            catch( ... )
            {
            }
            this->setStatus_( OpButtonSessionStatus::PL_CLOSED );
            this->notifySessionClosed_();
        }
        else
        {
            this->setStatus_( OpButtonSessionStatus::PL_DISCONNECTED );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::doShutdown()
    {
        OpButtonLog::i( "Shutting down OpButton SDK" );
        m_liveManager.shutdown();
        this->stopConnection();
        m_httpClient.shutdown();
        {
            std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
            m_actionListeners.clear();
            m_sessionListeners.clear();
        }
        {
            std::lock_guard<OpButtonMutex> lock( m_logBufferMutex );
            m_logBuffer.clear();
        }
        m_metrics.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::bufferLog( const OpButtonLogEntry & _entry )
    {
        bool warnOverflow = false;
        {
            std::lock_guard<OpButtonMutex> lock( m_logBufferMutex );
            if( static_cast<int>( m_logBuffer.size() ) >= m_config.logConfig().bufferSize() )
            {
                m_logBuffer.erase( m_logBuffer.begin() );
                if( m_logOverflowWarned == false )
                {
                    m_logOverflowWarned = true;
                    warnOverflow = true;
                }
            }
            m_logBuffer.push_back( _entry );
        }

        if( warnOverflow == true )
        {
            OpButtonLog::w( "Log buffer overflow (%d), dropping oldest entries", m_config.logConfig().bufferSize() );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonInstance::resolveLogAttributes()
    {
        const OpButtonLogAttributeBuilder & builder = m_config.logConfig().attributes();
        const OpButtonMap<OpButtonString, OpButtonString> & staticAttrs = builder.staticAttributes();
        const OpButtonMap<OpButtonString, OpButtonValueProvider *> & dynamicAttrs = builder.dynamicAttributes();

        if( staticAttrs.empty() && dynamicAttrs.empty() )
        {
            return OpButtonJson();
        }

        OpButtonJson attributes = OpButtonJson::object();
        for( const auto & kv : staticAttrs )
        {
            attributes.put( kv.first, OpButtonJson( kv.second ) );
        }
        for( const auto & kv : dynamicAttrs )
        {
            try
            {
                OpButtonString val = kv.second->getValue();
                attributes.put( kv.first, OpButtonJson( val ) );
            }
            catch( ... )
            {
            }
        }

        return attributes.objectSize() == 0 ? OpButtonJson() : attributes;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::dispatchQueuedActions()
    {
        OpButtonVector<OpButtonAction> actions;
        {
            std::lock_guard<OpButtonMutex> lock( m_actionQueueMutex );
            actions.swap( m_actionQueue );
        }

        OpButtonString token;
        {
            std::lock_guard<OpButtonMutex> lock( m_sessionTokenMutex );
            token = m_sessionToken;
        }

        for( const auto & action : actions )
        {
            // Intercept LIVE_* actions internally
            OpButtonActionType type = action.actionType();
            if( type == OpButtonActionType::LIVE_START || type == OpButtonActionType::LIVE_UPDATE || type == OpButtonActionType::LIVE_STOP || type == OpButtonActionType::LIVE_TAP ||
                type == OpButtonActionType::LIVE_LONG_PRESS )
            {
                OpButtonJson ack;
                switch( type )
                {
                case OpButtonActionType::LIVE_START:
                    ack = m_liveManager.handleStart( token, action.payload() );
                    break;
                case OpButtonActionType::LIVE_UPDATE:
                    ack = m_liveManager.handleUpdate( token, action.payload() );
                    break;
                case OpButtonActionType::LIVE_STOP:
                    ack = m_liveManager.handleStop();
                    break;
                case OpButtonActionType::LIVE_TAP:
                    ack = m_liveManager.handleTap( action.payload() );
                    break;
                case OpButtonActionType::LIVE_LONG_PRESS:
                    ack = m_liveManager.handleLongPress( action.payload() );
                    break;
                default:
                    break;
                }

                if( !ack.isNull() && !action.id().empty() && !token.empty() )
                {
                    try
                    {
                        m_httpClient.acknowledgeAction( token, action.id(), ack );
                    }
                    catch( const OpButtonException & e )
                    {
                        OpButtonLog::e( "Failed to acknowledge live action: %s", e.what() );
                    }
                }
                continue;
            }

            bool handled = false;
            try
            {
                handled = m_ui.dispatchAction( action );
            }
            catch( ... )
            {
                OpButtonLog::e( "Widget callback threw exception" );
            }

            if( handled == false )
            {
                std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
                for( auto * listener : m_actionListeners )
                {
                    try
                    {
                        listener->onOpButtonActionReceived( action );
                    }
                    catch( ... )
                    {
                        OpButtonLog::e( "Action listener threw exception" );
                    }
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::interruptibleSleep_( int64_t _ms )
    {
        std::unique_lock<OpButtonMutex> lock( m_sleepMutex );
        m_sleepCv.wait_for( lock, std::chrono::milliseconds( _ms ), [this]
                            { return !m_running.load(); } );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::connectAndWaitApproval_()
    {
        int64_t retryDelayMs = 2000;
        const int64_t maxRetryDelayMs = 30000;
        int retryCount = 0;

        while( m_running.load() == true )
        {
            try
            {
                this->doConnectAndWaitApproval_();
                return;
            }
            catch( const OpButtonException & e )
            {
                if( e.isUnauthorized() == true )
                {
                    OpButtonLog::e( "Authentication failed: %s", e.what() );
                    this->setStatus_( OpButtonSessionStatus::PL_AUTH_FAILED );
                    m_running.store( false );
                    this->notifyAuthFailed_();
                    return;
                }
                if( e.isNetworkError() == false )
                {
                    OpButtonLog::e( "Server error, stopping connection: %s", e.what() );
                    this->setStatus_( OpButtonSessionStatus::PL_ERROR );
                    m_running.store( false );
                    this->notifyError_( e );
                    return;
                }

                retryCount++;
                OpButtonLog::w( "Connection attempt %d failed (network): %s, retrying in %lldms", retryCount, e.what(), static_cast<long long>( retryDelayMs ) );
                this->setStatus_( OpButtonSessionStatus::PL_CONNECTING );
                this->interruptibleSleep_( retryDelayMs );
                retryDelayMs = std::min( retryDelayMs * 2, maxRetryDelayMs );
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::doConnectAndWaitApproval_()
    {
        this->setStatus_( OpButtonSessionStatus::PL_CONNECTING );
        this->notifyConnecting_();

        OpButtonString deviceId = m_config.deviceId();
        OpButtonString deviceName = m_config.deviceName();

        if( deviceId.empty() == true )
        {
            char compName[256] = {};
            DWORD size = sizeof( compName );
            GetComputerNameA( compName, &size );
            deviceId = compName;
        }
        if( deviceName.empty() == true )
        {
            char compName[256] = {};
            DWORD size = sizeof( compName );
            GetComputerNameA( compName, &size );
            deviceName = OpButtonString( compName ) + OpButtonString( " (Windows)" );
        }

        OpButtonConnectResponse resp = m_httpClient.connect( deviceId, deviceName, this->resolveAllSessionAttributes_() );

        OpButtonLog::i( "Connect request sent, request_id=%s, status=%s", resp.requestId().c_str(), resp.status().c_str() );

        if( resp.isApproved() && !resp.sessionToken().empty() )
        {
            this->onApproved_( resp.sessionToken() );
            return;
        }

        if( resp.isRejected() == true )
        {
            this->onRejected_();
            return;
        }

        this->setStatus_( OpButtonSessionStatus::PL_WAITING_APPROVAL );
        this->notifyWaitingApproval_( resp.requestId() );

        while( m_running.load() == true )
        {
            this->interruptibleSleep_( m_config.pollIntervalMs() );
            if( m_running.load() == false )
            {
                return;
            }

            OpButtonConnectResponse pollResp = m_httpClient.pollStatus( resp.requestId() );

            if( pollResp.isApproved() && !pollResp.sessionToken().empty() )
            {
                this->onApproved_( pollResp.sessionToken() );
                return;
            }

            if( pollResp.isRejected() == true )
            {
                this->onRejected_();
                return;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::onApproved_( const OpButtonString & _token )
    {
        {
            std::lock_guard<OpButtonMutex> lock( m_sessionTokenMutex );
            m_sessionToken = _token;
        }
        this->setStatus_( OpButtonSessionStatus::PL_ACTIVE );
        OpButtonLog::i( "Session approved and active" );
        this->notifySessionStarted_( _token );

        // Submit initial UI if available
        if( m_ui.hasTabs() == true )
        {
            OpButtonJson snapshot = m_ui.toJson();
            try
            {
                m_httpClient.submitPanel( _token, snapshot );
                m_ui.markSent();
                OpButtonLog::d( "Initial UI submitted (revision=%d)", m_ui.getRevision() );
            }
            catch( const OpButtonException & e )
            {
                OpButtonLog::e( "Failed to submit initial UI: %s", e.what() );
                this->notifyError_( e );
            }
        }

        // Start action polling thread
        m_actionPollThread = OpButtonThread( [this, _token]
                                             { this->actionPollLoop_( _token ); } );

        // Start metric sampling thread
        if( m_config.metricConfig().isEnabled() == true )
        {
            m_metricSampleThread = OpButtonThread( [this]
                                                   { this->metricSampleLoop_(); } );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::onRejected_()
    {
        OpButtonLog::w( "Connection request rejected" );
        this->setStatus_( OpButtonSessionStatus::PL_REJECTED );
        m_running.store( false );
        this->notifyRejected_();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::actionPollLoop_( const OpButtonString & _token )
    {
        while( m_running.load() == true )
        {
            this->doPollActions_( _token );
            this->interruptibleSleep_( m_currentActionPollIntervalMs );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::doPollActions_( const OpButtonString & _token )
    {
        try
        {
            OpButtonMap<OpButtonString, OpButtonJson> changedAttrs = this->resolveChangedSessionAttributes_();
            OpButtonVector<OpButtonLogEntry> logChunk = this->drainLogChunk_();
            OpButtonVector<OpButtonMetricEntry> metricChunk = m_metrics.drain();

            // Snapshot and submit UI if changed
            if( m_ui.hasUnsent() == true )
            {
                OpButtonJson uiSnapshot = m_ui.toJson();
                try
                {
                    m_httpClient.submitPanel( _token, uiSnapshot );
                    m_ui.markSent();
                    OpButtonLog::d( "UI submitted (revision=%d)", m_ui.getRevision() );
                }
                catch( const OpButtonException & e )
                {
                    OpButtonLog::e( "Failed to submit UI: %s", e.what() );
                    this->notifyError_( e );
                }
            }

            const OpButtonMap<OpButtonString, OpButtonJson> * changedPtr = changedAttrs.empty() ? nullptr : &changedAttrs;
            OpButtonJson json = m_httpClient.pollActions( _token, changedPtr, logChunk, metricChunk );
            OpButtonJson actionsArr = json.optArray( "actions" );

            if( actionsArr.length() > 0 )
            {
                std::lock_guard<OpButtonMutex> lock( m_actionQueueMutex );
                for( int i = 0; i < actionsArr.length(); ++i )
                {
                    OpButtonJson actionJson = actionsArr.at( i );
                    if( actionJson.isNull() == false )
                    {
                        m_actionQueue.push_back( OpButtonAction::fromJson( actionJson ) );
                    }
                }
            }

            if( logChunk.empty() == false )
            {
                std::lock_guard<OpButtonMutex> lock( m_logBufferMutex );
                m_logOverflowWarned = false;
            }
        }
        catch( const OpButtonException & e )
        {
            if( e.isNetworkError() == true )
            {
                OpButtonLog::w( "Network error during action poll, will retry" );
            }
            else
            {
                OpButtonLog::w( "Action poll failed with server error, attempting reconnect" );
                this->attemptReconnect_( _token );
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::metricSampleLoop_()
    {
        while( m_running.load() == true )
        {
            m_metrics.sample();
            this->interruptibleSleep_( m_metrics.sampleIntervalMs );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMap<OpButtonString, OpButtonJson> OpButtonInstance::resolveAllSessionAttributes_()
    {
        const OpButtonSessionAttributeBuilder & builder = m_config.sessionAttributes();
        OpButtonMap<OpButtonString, OpButtonJson> merged = builder.staticAttributes();

        for( const auto & kv : builder.dynamicAttributes() )
        {
            try
            {
                OpButtonString val = kv.second->getValue();
                OpButtonJson resolved( val );
                merged[kv.first] = resolved;
                m_sessionAttributeCache[kv.first] = resolved;
            }
            catch( ... )
            {
                OpButtonLog::e( "Session attribute provider failed: %s", kv.first.c_str() );
            }
        }

        return merged;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonMap<OpButtonString, OpButtonJson> OpButtonInstance::resolveChangedSessionAttributes_()
    {
        const OpButtonMap<OpButtonString, OpButtonValueProvider *> & dynamicAttrs = m_config.sessionAttributes().dynamicAttributes();
        if( dynamicAttrs.empty() == true )
        {
            return {};
        }

        OpButtonMap<OpButtonString, OpButtonJson> changed;

        for( const auto & kv : dynamicAttrs )
        {
            try
            {
                OpButtonString val = kv.second->getValue();
                OpButtonJson resolved( val );
                OpButtonMap<OpButtonString, OpButtonJson>::iterator cached = m_sessionAttributeCache.find( kv.first );
                if( cached == m_sessionAttributeCache.end() || cached->second.asString() != val )
                {
                    m_sessionAttributeCache[kv.first] = resolved;
                    changed[kv.first] = resolved;
                }
            }
            catch( ... )
            {
                OpButtonLog::e( "Session attribute provider failed: %s", kv.first.c_str() );
            }
        }

        return changed;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonVector<OpButtonLogEntry> OpButtonInstance::drainLogChunk_()
    {
        std::lock_guard<OpButtonMutex> lock( m_logBufferMutex );
        if( m_logBuffer.empty() == true )
        {
            return {};
        }

        int count = std::min( static_cast<int>( m_logBuffer.size() ), m_config.logConfig().batchSize() );
        OpButtonVector<OpButtonLogEntry> chunk( m_logBuffer.begin(), m_logBuffer.begin() + count );
        m_logBuffer.erase( m_logBuffer.begin(), m_logBuffer.begin() + count );
        return chunk;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::requeueLogs_( const OpButtonVector<OpButtonLogEntry> & _chunk )
    {
        if( _chunk.empty() == true )
        {
            return;
        }
        std::lock_guard<OpButtonMutex> lock( m_logBufferMutex );
        m_logBuffer.insert( m_logBuffer.begin(), _chunk.begin(), _chunk.end() );
        while( static_cast<int>( m_logBuffer.size() ) > m_config.logConfig().bufferSize() )
        {
            m_logBuffer.pop_back();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::flushLogs_( const OpButtonString & _token )
    {
        OpButtonVector<OpButtonLogEntry> chunk = this->drainLogChunk_();
        if( chunk.empty() == true )
        {
            return;
        }
        try
        {
            m_httpClient.sendLogs( _token, chunk );
        }
        catch( const OpButtonException & )
        {
            this->requeueLogs_( chunk );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::flushMetrics_( const OpButtonString & _token )
    {
        OpButtonVector<OpButtonMetricEntry> chunk = m_metrics.drain();
        if( chunk.empty() == true )
        {
            return;
        }
        try
        {
            m_httpClient.sendMetrics( _token, chunk );
        }
        catch( const OpButtonException & )
        {
            m_metrics.requeue( chunk );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::attemptReconnect_( const OpButtonString & _token )
    {
        this->setStatus_( OpButtonSessionStatus::PL_CONNECTING );

        int64_t retryDelay = 2000;
        const int64_t maxDelay = 30000;

        while( m_running.load() == true )
        {
            this->interruptibleSleep_( retryDelay );
            if( m_running.load() == false )
            {
                return;
            }

            try
            {
                m_httpClient.pollActions( _token, nullptr, {}, {} );
                OpButtonLog::i( "Reconnect successful, session still active" );
                this->setStatus_( OpButtonSessionStatus::PL_ACTIVE );
                return;
            }
            catch( const OpButtonException & e )
            {
                if( e.isNetworkError() == true )
                {
                    OpButtonLog::w( "Reconnect: no network, retrying in %lldms", static_cast<long long>( retryDelay ) );
                    retryDelay = std::min( retryDelay * 2, maxDelay );
                }
                else
                {
                    OpButtonLog::w( "Session lost (server error), starting fresh connection" );
                    this->resetAndRestart_();
                    return;
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::resetAndRestart_()
    {
        m_running.store( false );
        {
            std::lock_guard<OpButtonMutex> lock( m_sessionTokenMutex );
            m_sessionToken.clear();
        }
        this->setStatus_( OpButtonSessionStatus::PL_DISCONNECTED );
        this->notifySessionClosed_();
        this->startConnection();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::notifyConnecting_()
    {
        std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
        for( auto * l : m_sessionListeners )
        {
            l->onOpButtonSessionConnecting();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::notifyWaitingApproval_( const OpButtonString & _reqId )
    {
        std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
        for( auto * l : m_sessionListeners )
        {
            l->onOpButtonSessionWaitingApproval( _reqId );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::notifySessionStarted_( const OpButtonString & _token )
    {
        std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
        for( auto * l : m_sessionListeners )
        {
            l->onOpButtonSessionStarted( _token );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::notifySessionClosed_()
    {
        std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
        for( auto * l : m_sessionListeners )
        {
            l->onOpButtonSessionClosed();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::notifyRejected_()
    {
        std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
        for( auto * l : m_sessionListeners )
        {
            l->onOpButtonSessionRejected();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::notifyAuthFailed_()
    {
        std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
        for( auto * l : m_sessionListeners )
        {
            l->onOpButtonSessionAuthFailed();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonInstance::notifyError_( const OpButtonException & _e )
    {
        std::lock_guard<OpButtonMutex> lock( m_listenerMutex );
        for( auto * l : m_sessionListeners )
        {
            l->onOpButtonSessionError( _e );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSdk::~OpButtonSdk()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSdk::OpButtonSdk()
    {
    }
} // namespace OpButton
