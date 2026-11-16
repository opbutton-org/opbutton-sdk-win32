#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonSelect.h"
#include "OpButton/OpButtonRadioGroup.h"
#include "OpButton/OpButtonSlider.h"
#include <cstdlib>
#include <fstream>
#include <set>
#include <stdexcept>

namespace
{
    using namespace OpButton;

    void check( bool _result )
    {
        if( _result == false )
        {
            throw std::runtime_error( "Layout contract failed" );
        }
    }

    void collect( const OpButtonJson & _node, std::set<int> & _ids )
    {
        check( _node.has( "id" ) );
        check( _ids.insert( _node.optInt( "id" ) ).second );
        OpButtonJson children = _node.optArray( "children" );
        for( int i = 0; i != children.length(); ++i )
        {
            collect( children.at( i ), _ids );
        }
    }
} // namespace

int main()
{
    using namespace OpButton;
    OpButtonUI ui;
    OpButtonTab & retainedTab = ui.addTab( "Layout contract" );
    OpButtonLayout & root = retainedTab.vertical().gap( 12 ).padding( 8 ).density( "compact" );
    OpButtonLayout & row = root.addHorizontal().gap( 8 ).align( "end" ).justify( "space-between" ).wrap( true );
    OpButtonButton & button = row.addButton( "Apply" );
    button.width( 140 );
    row.addSelect( "Language" ).options( { { "en", "English" }, { "ru", "Русский" } } ).defaultValue( "en" ).fill( 2 ).minWidth( 160 ).maxWidth( 480 );
    OpButtonRadioGroup & radio = root.addRadioGroup( "Difficulty" ).options( { { "easy", "Easy" }, { "hard", "Hard" } } ).defaultValue( "easy" ).optionDirection( "horizontal" );
    OpButtonSlider & slider = root.addSlider( "Speed" ).range( .5, 3, .1 ).defaultValue( 1.5 ).unit( "x" );
    slider.fill();
    root.addCollapsible( "Advanced" ).gap( 4 ).density( "comfortable" ).addGroup( "Group" ).addButton( "Inside" );
    OpButtonLayout & grid = root.addGrid().columns( 3 ).minCellWidth( 140 ).gap( 8 );
    for( const char * text : { "A", "B", "C" } )
    {
        grid.addButton( text );
    }
    int identifier = button.internalId();
    ui.markSent();
    button.width( 160 );
    check( ui.hasUnsent() );
    check( identifier == button.internalId() );
    OpButtonJson layout = ui.toJson();
    OpButtonJson tab = layout.optArray( "tabs" ).at( 0 );
    std::set<int> ids{ tab.optInt( "id" ) };
    collect( tab.optObject( "layout" ), ids );
    check( root.toJson().optArray( "children" ).at( 3 ).optArray( "children" ).at( 0 ).optDouble( "gap" ) == 4 );
    double received = 0;
    slider.onChange( OpButtonFunction<void( const OpButtonSliderAction & )>( [&received]( const OpButtonSliderAction & _action )
                                                                             { received = _action.value(); } ) );
    check( ui.dispatchAction( OpButtonAction( "action", "session", slider.internalId(), OpButtonActionType::CHANGE,
                                              OpButtonActionStatus::DELIVERED, OpButtonJson::object().put( "value", OpButtonJson( 1.7 ) ) ) ) );
    check( received == 1.7 );
    OpButtonString choice;
    radio.onChange( OpButtonFunction<void( const OpButtonRadioGroupAction & )>( [&choice]( const OpButtonRadioGroupAction & _action )
                                                                                { choice = _action.value(); } ) );
    ui.dispatchAction( OpButtonAction( "action", "session", radio.internalId(), OpButtonActionType::CHANGE,
                                       OpButtonActionStatus::DELIVERED, OpButtonJson::object().put( "value", OpButtonJson( "hard" ) ) ) );
    check( choice == "hard" );
    const char * directory = std::getenv( "OPBUTTON_LAYOUT_FIXTURE_DIR" );
    if( directory != nullptr )
    {
        std::ofstream file( std::string( directory ) + "/cpp.json" );
        file << layout.toString().c_str();
        check( file.good() );
    }
    int removedId = slider.internalId();
    ui.markSent();
    OpButtonLayout & replacement = retainedTab.grid();
    check( ui.hasUnsent() );
    check( replacement.internalId() > removedId );
    check( ui.dispatchAction( OpButtonAction( "late", "session", removedId, OpButtonActionType::CHANGE, OpButtonActionStatus::DELIVERED, OpButtonJson::object() ) ) == false );
    replacement.addPadding( 0 );
    return 0;
}
