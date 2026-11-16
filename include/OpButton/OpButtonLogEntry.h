#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonLogLevel.h"
#include <cstdint>

namespace OpButton
{
    class OpButtonLogEntry
    {
    public:
        OpButtonLogEntry( OpButtonLogLevel _level, const OpButtonString & _message, const OpButtonString & _category = OpButtonString(), const OpButtonString & _thread = OpButtonString(), const OpButtonMap<OpButtonString, OpButtonJson> & _metadata = {}, const OpButtonJson & _attributes = OpButtonJson(), int64_t _clientTimestampMs = 0 );
        ~OpButtonLogEntry();

        OpButtonLogEntry( const OpButtonString & _level, const OpButtonString & _message, const OpButtonString & _category = OpButtonString(), const OpButtonString & _thread = OpButtonString(), const OpButtonMap<OpButtonString, OpButtonJson> & _metadata = {}, const OpButtonJson & _attributes = OpButtonJson(), int64_t _clientTimestampMs = 0 );

        OpButtonJson toJson() const;

        static OpButtonLogEntry debug( const OpButtonString & _msg );
        static OpButtonLogEntry info( const OpButtonString & _msg );
        static OpButtonLogEntry warning( const OpButtonString & _msg );
        static OpButtonLogEntry error( const OpButtonString & _msg );
        static OpButtonLogEntry critical( const OpButtonString & _msg );

    private:
        OpButtonString m_level;
        OpButtonString m_message;
        OpButtonString m_category;
        OpButtonString m_thread;
        OpButtonMap<OpButtonString, OpButtonJson> m_metadata;
        OpButtonJson m_attributes;
        OpButtonString m_clientTimestamp;

        static OpButtonString formatTimestamp( int64_t _ms );
        static int64_t currentTimeMs();
    };

} // namespace OpButton
