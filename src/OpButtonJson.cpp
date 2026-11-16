#include "OpButton/OpButtonJson.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson & OpButtonJson::put( const OpButtonString & _key, const OpButtonJson & _value )
    {
        if( m_type != TYPE_OBJECT )
        {
            m_type = TYPE_OBJECT;
        }
        m_object[_key] = _value;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonJson::has( const OpButtonString & _key ) const
    {
        return m_type == TYPE_OBJECT && m_object.find( _key ) != m_object.end();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::get( const OpButtonString & _key ) const
    {
        if( m_type != TYPE_OBJECT )
        {
            return OpButtonJson();
        }
        OpButtonMap<OpButtonString, OpButtonJson>::const_iterator it = m_object.find( _key );
        return it != m_object.end() ? it->second : OpButtonJson();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonJson::optString( const OpButtonString & _key, const OpButtonString & _def ) const
    {
        OpButtonJson v = this->get( _key );
        return v.m_type == TYPE_STRING ? v.m_string : _def;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonJson::optInt( const OpButtonString & _key, int _def ) const
    {
        OpButtonJson v = this->get( _key );
        if( v.m_type == TYPE_INT )
        {
            return static_cast<int>( v.m_int );
        }
        if( v.m_type == TYPE_DOUBLE )
        {
            return static_cast<int>( v.m_double );
        }
        return _def;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonJson::optBool( const OpButtonString & _key, bool _def ) const
    {
        OpButtonJson v = this->get( _key );
        return v.m_type == TYPE_BOOL ? v.m_bool : _def;
    }

    //////////////////////////////////////////////////////////////////////////
    double OpButtonJson::optDouble( const OpButtonString & _key, double _def ) const
    {
        OpButtonJson v = this->get( _key );
        if( v.m_type == TYPE_DOUBLE )
        {
            return v.m_double;
        }
        if( v.m_type == TYPE_INT )
        {
            return static_cast<double>( v.m_int );
        }
        return _def;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::optObject( const OpButtonString & _key ) const
    {
        OpButtonJson v = this->get( _key );
        return v.m_type == TYPE_OBJECT ? v : OpButtonJson::object();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::optArray( const OpButtonString & _key ) const
    {
        OpButtonJson v = this->get( _key );
        return v.m_type == TYPE_ARRAY ? v : OpButtonJson::array();
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonJson::objectSize() const
    {
        return m_type == TYPE_OBJECT ? static_cast<int>( m_object.size() ) : 0;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson & OpButtonJson::add( const OpButtonJson & _value )
    {
        if( m_type != TYPE_ARRAY )
        {
            m_type = TYPE_ARRAY;
        }
        m_array.push_back( _value );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonJson::length() const
    {
        return m_type == TYPE_ARRAY ? static_cast<int>( m_array.size() ) : 0;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::at( int _index ) const
    {
        if( m_type != TYPE_ARRAY || _index < 0 || _index >= static_cast<int>( m_array.size() ) )
        {
            return OpButtonJson();
        }
        return m_array[_index];
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonJson::asBool() const
    {
        if( m_type == TYPE_BOOL )
        {
            return m_bool;
        }
        if( m_type == TYPE_INT )
        {
            return m_int != 0;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonJson::asInt() const
    {
        if( m_type == TYPE_INT )
        {
            return static_cast<int>( m_int );
        }
        if( m_type == TYPE_DOUBLE )
        {
            return static_cast<int>( m_double );
        }
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    int64_t OpButtonJson::asInt64() const
    {
        if( m_type == TYPE_INT )
        {
            return m_int;
        }
        if( m_type == TYPE_DOUBLE )
        {
            return static_cast<int64_t>( m_double );
        }
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    double OpButtonJson::asDouble() const
    {
        if( m_type == TYPE_DOUBLE )
        {
            return m_double;
        }
        if( m_type == TYPE_INT )
        {
            return static_cast<double>( m_int );
        }
        return 0.0;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonJson::asString() const
    {
        if( m_type == TYPE_STRING )
        {
            return m_string;
        }
        if( m_type == TYPE_INT )
        {
            return opbuttonToString( m_int );
        }
        if( m_type == TYPE_DOUBLE )
        {
            char buf[64];
            snprintf( buf, sizeof( buf ), "%g", m_double );
            return OpButtonString( buf );
        }
        if( m_type == TYPE_BOOL )
        {
            return m_bool ? OpButtonString( "true" ) : OpButtonString( "false" );
        }
        return OpButtonString();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::object()
    {
        OpButtonJson j;
        j.m_type = TYPE_OBJECT;
        return j;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::array()
    {
        OpButtonJson j;
        j.m_type = TYPE_ARRAY;
        return j;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::null_value()
    {
        return OpButtonJson();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonJson::escapeString( const OpButtonString & _s )
    {
        OpButtonString out;
        out.reserve( _s.size() + 2 );
        out += '"';
        for( char c : _s )
        {
            switch( c )
            {
            case '"':
                out += "\\\"";
                break;
            case '\\':
                out += "\\\\";
                break;
            case '\b':
                out += "\\b";
                break;
            case '\f':
                out += "\\f";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                if( static_cast<unsigned char>( c ) < 0x20 )
                {
                    char buf[8];
                    snprintf( buf, sizeof( buf ), "\\u%04x", static_cast<unsigned char>( c ) );
                    out += buf;
                }
                else
                {
                    out += c;
                }
            }
        }
        out += '"';
        return out;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonJson::writeJson_( OpButtonString & _out ) const
    {
        switch( m_type )
        {
        case TYPE_NULL:
            _out += "null";
            break;
        case TYPE_BOOL:
            _out += m_bool ? "true" : "false";
            break;
        case TYPE_INT:
            _out += opbuttonToString( m_int );
            break;
        case TYPE_DOUBLE:
        {
            char buf[64];
            snprintf( buf, sizeof( buf ), "%g", m_double );
            _out += buf;
            break;
        }
        case TYPE_STRING:
            _out += escapeString( m_string );
            break;
        case TYPE_ARRAY:
        {
            _out += '[';
            for( size_t i = 0; i < m_array.size(); ++i )
            {
                if( i > 0 )
                {
                    _out += ',';
                }
                m_array[i].writeJson_( _out );
            }
            _out += ']';
            break;
        }
        case TYPE_OBJECT:
        {
            _out += '{';
            bool first = true;
            for( const auto & kv : m_object )
            {
                if( first == false )
                {
                    _out += ',';
                }
                first = false;
                _out += escapeString( kv.first );
                _out += ':';
                kv.second.writeJson_( _out );
            }
            _out += '}';
            break;
        }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonJson::toString() const
    {
        OpButtonString out;
        this->writeJson_( out );
        return out;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::parse( const OpButtonString & _str )
    {
        Parser p;
        p.data = _str.c_str();
        p.pos = 0;
        p.len = _str.size();
        p.skipWhitespace();
        if( p.pos >= p.len )
        {
            return OpButtonJson();
        }
        return p.parseValue();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonJson::Parser::skipWhitespace()
    {
        while( pos < len && ( data[pos] == ' ' || data[pos] == '\t' || data[pos] == '\n' || data[pos] == '\r' ) )
        {
            ++pos;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    char OpButtonJson::Parser::peek()
    {
        return pos < len ? data[pos] : '\0';
    }

    //////////////////////////////////////////////////////////////////////////
    char OpButtonJson::Parser::advance()
    {
        return pos < len ? data[pos++] : '\0';
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::Parser::parseValue()
    {
        this->skipWhitespace();
        char c = this->peek();
        if( c == '{' )
        {
            return this->parseObject();
        }
        if( c == '[' )
        {
            return this->parseArray();
        }
        if( c == '"' )
        {
            return OpButtonJson( this->parseStringValue() );
        }
        if( c == '-' || ( c >= '0' && c <= '9' ) )
        {
            return this->parseNumber();
        }
        return this->parseLiteral();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::Parser::parseObject()
    {
        this->advance(); // '{'
        OpButtonJson obj = OpButtonJson::object();
        this->skipWhitespace();
        if( this->peek() == '}' )
        {
            this->advance();
            return obj;
        }

        while( true )
        {
            this->skipWhitespace();
            if( this->peek() != '"' )
            {
                break;
            }
            OpButtonString key = this->parseStringValue();
            this->skipWhitespace();
            if( this->peek() == ':' )
            {
                this->advance();
            }
            OpButtonJson value = this->parseValue();
            obj.put( key, value );
            this->skipWhitespace();
            if( this->peek() == ',' )
            {
                this->advance();
                continue;
            }
            break;
        }

        this->skipWhitespace();
        if( this->peek() == '}' )
        {
            this->advance();
        }
        return obj;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::Parser::parseArray()
    {
        this->advance(); // '['
        OpButtonJson arr = OpButtonJson::array();
        this->skipWhitespace();
        if( this->peek() == ']' )
        {
            this->advance();
            return arr;
        }

        while( true )
        {
            arr.add( this->parseValue() );
            this->skipWhitespace();
            if( this->peek() == ',' )
            {
                this->advance();
                continue;
            }
            break;
        }

        this->skipWhitespace();
        if( this->peek() == ']' )
        {
            this->advance();
        }
        return arr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonJson::Parser::parseStringValue()
    {
        this->advance(); // '"'
        OpButtonString result;
        while( pos < len )
        {
            char c = this->advance();
            if( c == '"' )
            {
                break;
            }
            if( c == '\\' )
            {
                char next = this->advance();
                switch( next )
                {
                case '"':
                    result += '"';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '/':
                    result += '/';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'u':
                {
                    char hex[5] = {};
                    for( int i = 0; i < 4 && pos < len; ++i )
                    {
                        hex[i] = this->advance();
                    }
                    unsigned int codepoint = static_cast<unsigned int>( strtoul( hex, nullptr, 16 ) );
                    if( codepoint < 0x80 )
                    {
                        result += static_cast<char>( codepoint );
                    }
                    else if( codepoint < 0x800 )
                    {
                        result += static_cast<char>( 0xC0 | ( codepoint >> 6 ) );
                        result += static_cast<char>( 0x80 | ( codepoint & 0x3F ) );
                    }
                    else
                    {
                        result += static_cast<char>( 0xE0 | ( codepoint >> 12 ) );
                        result += static_cast<char>( 0x80 | ( ( codepoint >> 6 ) & 0x3F ) );
                        result += static_cast<char>( 0x80 | ( codepoint & 0x3F ) );
                    }
                    break;
                }
                default:
                    result += next;
                    break;
                }
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::Parser::parseNumber()
    {
        size_t start = pos;
        bool isDouble = false;

        if( this->peek() == '-' )
        {
            this->advance();
        }
        while( pos < len && data[pos] >= '0' && data[pos] <= '9' )
        {
            this->advance();
        }

        if( pos < len && data[pos] == '.' )
        {
            isDouble = true;
            this->advance();
            while( pos < len && data[pos] >= '0' && data[pos] <= '9' )
            {
                this->advance();
            }
        }

        if( pos < len && ( data[pos] == 'e' || data[pos] == 'E' ) )
        {
            isDouble = true;
            this->advance();
            if( pos < len && ( data[pos] == '+' || data[pos] == '-' ) )
            {
                this->advance();
            }
            while( pos < len && data[pos] >= '0' && data[pos] <= '9' )
            {
                this->advance();
            }
        }

        OpButtonString numStr( data + start, pos - start );
        if( isDouble == true )
        {
            return OpButtonJson( strtod( numStr.c_str(), nullptr ) );
        }
        else
        {
            int64_t val = strtoll( numStr.c_str(), nullptr, 10 );
            return OpButtonJson( val );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJson::Parser::parseLiteral()
    {
        if( pos + 4 <= len && strncmp( data + pos, "true", 4 ) == 0 )
        {
            pos += 4;
            return OpButtonJson( true );
        }
        if( pos + 5 <= len && strncmp( data + pos, "false", 5 ) == 0 )
        {
            pos += 5;
            return OpButtonJson( false );
        }
        if( pos + 4 <= len && strncmp( data + pos, "null", 4 ) == 0 )
        {
            pos += 4;
            return OpButtonJson();
        }
        this->advance();
        return OpButtonJson();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson()
        : m_type( TYPE_NULL )
        , m_bool( false )
        , m_int( 0 )
        , m_double( 0.0 )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( bool _v )
        : m_type( TYPE_BOOL )
        , m_bool( _v )
        , m_int( 0 )
        , m_double( 0.0 )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( int _v )
        : m_type( TYPE_INT )
        , m_bool( false )
        , m_int( _v )
        , m_double( 0.0 )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( int64_t _v )
        : m_type( TYPE_INT )
        , m_bool( false )
        , m_int( _v )
        , m_double( 0.0 )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( double _v )
        : m_type( TYPE_DOUBLE )
        , m_bool( false )
        , m_int( 0 )
        , m_double( _v )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( const char * _v )
        : m_type( TYPE_STRING )
        , m_bool( false )
        , m_int( 0 )
        , m_double( 0.0 )
        , m_string( _v ? _v : "" )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( const OpButtonString & _v )
        : m_type( TYPE_STRING )
        , m_bool( false )
        , m_int( 0 )
        , m_double( 0.0 )
        , m_string( _v )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::Type OpButtonJson::type() const
    {
        return m_type;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonJson::isNull() const
    {
        return m_type == TYPE_NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonMap<OpButtonString, OpButtonJson> & OpButtonJson::objectEntries() const
    {
        return m_object;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::~OpButtonJson()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::Parser::Parser()
        : data()
        , pos()
        , len()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::Parser::~Parser()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::Parser::Parser( const char * _data, size_t _pos, size_t _len )
        : data( _data )
        , pos( _pos )
        , len( _len )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( const OpButtonJson & _other ) = default;
    //////////////////////////////////////////////////////////////////////////
    OpButtonJson::OpButtonJson( OpButtonJson && _other ) noexcept = default;
    //////////////////////////////////////////////////////////////////////////
    OpButtonJson & OpButtonJson::operator=( const OpButtonJson & _other ) = default;
    //////////////////////////////////////////////////////////////////////////
    OpButtonJson & OpButtonJson::operator=( OpButtonJson && _other ) noexcept = default;
} // namespace OpButton
