#include "OpButton/OpButtonConfig.h"

#include <functional>
#include <stdexcept>

namespace
{
    using namespace OpButton;

    void check( bool _value )
    {
        if( _value == false )
        {
            throw std::runtime_error( "Security contract failed" );
        }
    }

    void expectInvalid( const std::function<void()> & _operation )
    {
        bool rejected = false;
        try
        {
            _operation();
        }
        catch( const std::invalid_argument & )
        {
            rejected = true;
        }
        check( rejected );
    }
}

int main()
{
    using namespace OpButton;

    expectInvalid( []() { OpButtonConfig::Builder( "", "token" ).build(); } );
    expectInvalid( []() { OpButtonConfig::Builder( "https://example.com", "" ).build(); } );
    expectInvalid( []() { OpButtonConfig::Builder( "https://example.com", "token\r\ninjected: value" ).build(); } );
    expectInvalid( []() { OpButtonConfig::Builder( "https://example.com", "téken" ).build(); } );
    expectInvalid( []() { OpButtonConfig::Builder( "https://example.com", "token" ).setPollIntervalMs(249).build(); } );
    expectInvalid( []() { OpButtonConfig::Builder( "https://example.com", "token" ).setDeviceSecret( "short" ).build(); } );
    expectInvalid( []() {
        OpButtonConfig::Builder( "https://example.com", "token" )
            .setDeviceSecret( OpButtonString( 32, 'x' ) + "\n" )
            .build();
    } );

    OpButtonConfig config = OpButtonConfig::Builder( "https://example.com", "token" )
        .setDeviceSecret( OpButtonString( 32, 'x' ) )
        .build();
    check( config.deviceSecret().size() == 32 );
}
