#pragma once

#include "OpButton/OpButtonTypes.h"
#include <cstdint>

namespace OpButton
{
    class OpButtonJson
    {
    public:
        OpButtonJson();
        OpButtonJson( bool _v );
        OpButtonJson( int _v );
        OpButtonJson( int64_t _v );
        OpButtonJson( double _v );
        OpButtonJson( const char * _v );
        OpButtonJson( const OpButtonString & _v );
        OpButtonJson( const OpButtonJson & _other );
        OpButtonJson( OpButtonJson && _other ) noexcept;
        ~OpButtonJson();

    public:
        enum Type
        {
            TYPE_NULL,
            TYPE_BOOL,
            TYPE_INT,
            TYPE_DOUBLE,
            TYPE_STRING,
            TYPE_ARRAY,
            TYPE_OBJECT
        };

    public:
        OpButtonJson & operator=( const OpButtonJson & _other );
        OpButtonJson & operator=( OpButtonJson && _other ) noexcept;

        Type type() const;
        bool isNull() const;

        // Object operations
        OpButtonJson & put( const OpButtonString & _key, const OpButtonJson & _value );
        bool has( const OpButtonString & _key ) const;
        OpButtonJson get( const OpButtonString & _key ) const;
        OpButtonString optString( const OpButtonString & _key, const OpButtonString & _def = OpButtonString() ) const;
        int optInt( const OpButtonString & _key, int _def = 0 ) const;
        bool optBool( const OpButtonString & _key, bool _def = false ) const;
        double optDouble( const OpButtonString & _key, double _def = 0.0 ) const;
        OpButtonJson optObject( const OpButtonString & _key ) const;
        OpButtonJson optArray( const OpButtonString & _key ) const;
        int objectSize() const;
        const OpButtonMap<OpButtonString, OpButtonJson> & objectEntries() const;

        // Array operations
        OpButtonJson & add( const OpButtonJson & _value );
        int length() const;
        OpButtonJson at( int _index ) const;

        // Value access
        bool asBool() const;
        int asInt() const;
        int64_t asInt64() const;
        double asDouble() const;
        OpButtonString asString() const;

        // Serialization
        OpButtonString toString() const;

        // Parsing
        static OpButtonJson parse( const OpButtonString & _str );

        // Factory
        static OpButtonJson object();
        static OpButtonJson array();
        static OpButtonJson null_value();

    private:
        Type m_type;
        bool m_bool;
        int64_t m_int;
        double m_double;
        OpButtonString m_string;
        OpButtonVector<OpButtonJson> m_array;
        OpButtonMap<OpButtonString, OpButtonJson> m_object;

    private:
        void writeJson_( OpButtonString & _out ) const;
        static OpButtonString escapeString( const OpButtonString & _s );

    private:
        struct Parser
        {
        public:
            Parser();
            Parser( const char * _data, size_t _pos, size_t _len );
            ~Parser();

        public:
            const char * data;
            size_t pos;
            size_t len;

        public:
            OpButtonJson parseValue();
            OpButtonJson parseObject();
            OpButtonJson parseArray();
            OpButtonString parseStringValue();
            OpButtonJson parseNumber();
            OpButtonJson parseLiteral();
            void skipWhitespace();
            char peek();
            char advance();
        };
    };

} // namespace OpButton
