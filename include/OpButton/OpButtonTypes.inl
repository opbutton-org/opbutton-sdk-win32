#pragma once

namespace OpButton
{
    template<class T>
    OpButtonString opbuttonToString( T _value )
    {
        std::string value = std::to_string( _value );
        OpButtonString result( value.data(), value.size() );
        return result;
    }
} // namespace OpButton
