#include "OpButton/OpButtonLogEntry.h"

#include <chrono>
#include <ctime>

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    static OpButtonString formatTimestampImpl( int64_t _ms )
    {
        time_t seconds = static_cast<time_t>( _ms / 1000 );
        struct tm utcBuf;
#ifdef _WIN32
        gmtime_s( &utcBuf, &seconds );
#else
        gmtime_r( &seconds, &utcBuf );
#endif
        char buf[32];
        strftime( buf, sizeof( buf ), "%Y-%m-%dT%H:%M:%SZ", &utcBuf );
        return buf;
    }

    //////////////////////////////////////////////////////////////////////////
    static int64_t currentTimeMsImpl()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry::OpButtonLogEntry( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonString & _category, const OpButtonString & _thread, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata, const OpButtonJson & _attributes, int64_t _clientTimestampMs )
        : m_level( opbuttonLogLevelValue( _level ) )
        , m_message( _message )
        , m_category( _category )
        , m_thread( _thread )
        , m_metadata( _metadata )
        , m_attributes( _attributes )
        , m_clientTimestamp( formatTimestampImpl( _clientTimestampMs > 0 ? _clientTimestampMs : currentTimeMsImpl() ) )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry::OpButtonLogEntry( const OpButtonString & _level, const OpButtonString & _message, const OpButtonString & _category, const OpButtonString & _thread, const OpButtonMap<OpButtonString, OpButtonJson> & _metadata, const OpButtonJson & _attributes, int64_t _clientTimestampMs )
        : m_level( _level )
        , m_message( _message )
        , m_category( _category )
        , m_thread( _thread )
        , m_metadata( _metadata )
        , m_attributes( _attributes )
        , m_clientTimestamp( formatTimestampImpl( _clientTimestampMs > 0 ? _clientTimestampMs : currentTimeMsImpl() ) )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLogEntry::toJson() const
    {
        OpButtonJson json = OpButtonJson::object();
        json.put( "level", OpButtonJson( m_level ) );
        json.put( "message", OpButtonJson( m_message ) );
        json.put( "client_timestamp", OpButtonJson( m_clientTimestamp ) );
        if( m_category.empty() == false )
        {
            json.put( "category", OpButtonJson( m_category ) );
        }
        if( m_thread.empty() == false )
        {
            json.put( "thread", OpButtonJson( m_thread ) );
        }
        if( m_metadata.empty() == false )
        {
            OpButtonJson metaJson = OpButtonJson::object();
            for( const auto & kv : m_metadata )
            {
                metaJson.put( kv.first, kv.second );
            }
            json.put( "metadata", metaJson );
        }
        if( m_attributes.isNull() == false )
        {
            json.put( "attributes", m_attributes );
        }
        return json;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonLogEntry::formatTimestamp( int64_t _ms )
    {
        return formatTimestampImpl( _ms );
    }

    //////////////////////////////////////////////////////////////////////////
    int64_t OpButtonLogEntry::currentTimeMs()
    {
        return currentTimeMsImpl();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry OpButtonLogEntry::debug( const OpButtonString & _msg )
    {
        return OpButtonLogEntry( OpButtonLogLevel::PL_DEBUG, _msg );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry OpButtonLogEntry::info( const OpButtonString & _msg )
    {
        return OpButtonLogEntry( OpButtonLogLevel::PL_INFO, _msg );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry OpButtonLogEntry::warning( const OpButtonString & _msg )
    {
        return OpButtonLogEntry( OpButtonLogLevel::PL_WARNING, _msg );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry OpButtonLogEntry::error( const OpButtonString & _msg )
    {
        return OpButtonLogEntry( OpButtonLogLevel::PL_ERROR, _msg );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry OpButtonLogEntry::critical( const OpButtonString & _msg )
    {
        return OpButtonLogEntry( OpButtonLogLevel::PL_CRITICAL, _msg );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogEntry::~OpButtonLogEntry()
    {
    }
} // namespace OpButton
