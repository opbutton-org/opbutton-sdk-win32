#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonButton.h"

#include <memory>
#include <set>
#include <stdexcept>

namespace
{
    using namespace OpButton;

    void check( bool _value )
    {
        if( _value == false )
        {
            throw std::runtime_error( "JSON panel contract failed" );
        }
    }

    void collect( const OpButtonJson & _node, std::set<int> & _ids )
    {
        check( _ids.insert( _node.optInt( "id" ) ).second );
        OpButtonJson children = _node.optArray( "children" );

        for( int index = 0; index != children.length(); ++index )
        {
            collect( children.at( index ), _ids );
        }
    }

    OpButtonAction action( int _id )
    {
        return OpButtonAction( "action", "session", _id, OpButtonActionType::CHANGE, OpButtonActionStatus::DELIVERED, OpButtonJson::object() );
    }
}

int main()
{
    using namespace OpButton;
    OpButtonUI ui;
    OpButtonTab & tab = ui.addTab( "json" ).setName( "JSON" );
    OpButtonJson description = OpButtonJson::parse( R"({"type":"layout","direction":"grid","gap":8,"gridColumns":3,"placement":{"width":"fill","weight":2},"future":{"enabled":true},"children":[{"type":"slider","id":"speed","label":"Speed","min":0.5,"max":3,"step":0.1}]})" );
    OpButtonString received;
    std::weak_ptr<int> lifetime;

    {
        std::shared_ptr<int> retained = std::make_shared<int>( 1 );
        lifetime = retained;
        tab.setJson( description, [&received, retained]( const OpButtonString & _identifier, const OpButtonAction & ) {
            (void)retained;
            received = _identifier;
        } ).padding( 4 ).addButton( "Native" );
    }

    OpButtonJson layout = tab.toJson().get( "layout" );
    check( layout.optInt( "gap" ) == 8 );
    check( layout.optInt( "padding" ) == 4 );
    check( layout.get( "future" ).optBool( "enabled" ) );
    check( layout.get( "placement" ).optInt( "weight" ) == 2 );
    std::set<int> ids;
    collect( layout, ids );
    int speedId = layout.get( "children" ).at( 0 ).optInt( "id" );
    check( ui.dispatchAction( action( speedId ) ) );
    check( received == "speed" );
    bool rejected = false;

    try
    {
        tab.setJson( OpButtonJson::parse( R"({"type":"layout","direction":"vertical","children":[{"type":"button","id":"first"},42]})" ) );
    }
    catch( const std::invalid_argument & )
    {
        rejected = true;
    }

    check( rejected );
    check( ui.dispatchAction( action( speedId ) ) );
    tab.setJson( description, [&ui]( const OpButtonString &, const OpButtonAction & ) { ui.removeTab( "json" ); } );
    check( lifetime.expired() );
    check( ui.dispatchAction( action( speedId ) ) == false );
    int nextId = tab.toJson().get( "layout" ).get( "children" ).at( 0 ).optInt( "id" );
    check( ui.dispatchAction( action( nextId ) ) );
    check( ui.getTab( "json" ) == nullptr );
    check( ui.dispatchAction( action( nextId ) ) == false );
}
