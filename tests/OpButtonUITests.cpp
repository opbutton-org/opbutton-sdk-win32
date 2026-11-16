#include "OpButton/OpButtonAction.h"
#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonUI.h"

#include <atomic>
#include <iostream>

#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace
{

    using namespace OpButton;

    void check( bool result, const char * message )
    {
        if( !result )
        {
            throw std::runtime_error( message );
        }
    }

    OpButtonAction actionFor( int widgetId )
    {
        return OpButtonAction( "action", "session", widgetId, OpButtonActionType::CLICK, OpButtonActionStatus::DELIVERED, OpButtonJson::object() );
    }

    struct DestructorAction
        : public OpButtonRefCounted
    {
        explicit DestructorAction( std::function<void()> value )
            : action( std::move( value ) )
        {
        }

        ~DestructorAction()
        {
            action();
        }

    public:
        std::function<void()> action;
    };

    class RawCallback
        : public OpButtonWidgetCallback
    {
    public:
        void onOpButtonWidgetAction( const OpButtonAction & action ) override
        {
            if( callback )
            {
                callback( action );
            }
        }

    public:
        OpButtonWidgetCallbackFunc callback;
    };

    void removeFromCallback()
    {
        OpButtonUI ui;
        auto & tab = ui.addTab( "Test" );
        tab.setId( "Test" );
        auto & button = tab.vertical().addButton( "Remove" );
        int id = button.internalId();
        bool called = false;
        button.onClick( OpButtonWidgetCallbackFunc(
            [&]( const OpButtonAction & )
            {
                OpButtonWidgetCallbackFunc empty;
                button.onClick( empty );
                ui.removeTab( "Test" );
                called = true;
            } ) );
        check( ui.dispatchAction( actionFor( id ) ), "remove callback was not dispatched" );
        check( called, "remove callback did not return" );
        check( ui.getTab( "Test" ) == nullptr, "tab was not removed" );
        check( !ui.dispatchAction( actionFor( id ) ), "removed handler remained registered" );
    }

    void addFromCallback()
    {
        OpButtonUI ui;
        auto & layout = ui.addTab( "Test" ).vertical();
        auto & button = layout.addButton( "Add" );
        int addedId = 0;
        int calls = 0;
        button.onClick( OpButtonWidgetCallbackFunc(
            [&]( const OpButtonAction & )
            {
                auto & added = layout.addButton( "Added" );
                addedId = added.internalId();
                added.onClick( OpButtonWidgetCallbackFunc( [&]( const OpButtonAction & )
                                                           { ++calls; } ) );
            } ) );
        check( ui.dispatchAction( actionFor( button.internalId() ) ), "add callback was not dispatched" );
        check( addedId != 0, "new button was not created" );
        check( ui.dispatchAction( actionFor( addedId ) ), "new handler was not registered" );
        check( calls == 1, "new handler was not called" );
    }

    void replaceFromCallback()
    {
        OpButtonUI ui;
        int oldCalls = 0;
        int newCalls = 0;
        ui.registerCallbackFunc( 1,
                                 [&]( const OpButtonAction & )
                                 {
                                     ++oldCalls;
                                     ui.registerCallbackFunc( 1, [&]( const OpButtonAction & )
                                                              { ++newCalls; } );
                                 } );
        ui.dispatchAction( actionFor( 1 ) );
        ui.dispatchAction( actionFor( 1 ) );
        check( oldCalls == 1 && newCalls == 1, "handler replacement failed" );
    }

    void preserveCallbackState()
    {
        OpButtonUI ui;
        int observed = 0;
        ui.registerCallbackFunc( 1, [count = 0, &observed]( const OpButtonAction & ) mutable
                                 { observed = ++count; } );
        ui.dispatchAction( actionFor( 1 ) );
        ui.dispatchAction( actionFor( 1 ) );
        check( observed == 2, "dispatch copied and reset mutable callback state" );
    }

    void retainDuringDispatch()
    {
        OpButtonUI ui;
        int destroyed = 0;
        auto owner = OpButtonIntrusivePtr<DestructorAction>( new DestructorAction( [&]
                                                                                   { ++destroyed; } ) );

        ui.registerCallbackFunc( 1,
                                 [&, owner]( const OpButtonAction & )
                                 {
                                     ui.removeCallback( 1 );
                                     check( destroyed == 0, "active handler owner was released during dispatch" );
                                 } );
        owner.reset();
        check( destroyed == 0, "handler owner was not retained" );
        ui.dispatchAction( actionFor( 1 ) );
        check( destroyed == 1, "handler owner survived completed dispatch" );
    }

    void reenterFromDestructor()
    {
        for( int mode = 0; mode < 5; ++mode )
        {
            OpButtonUI ui;
            RawCallback raw;
            int destroyed = 0;
            auto owner = OpButtonIntrusivePtr<DestructorAction>( new DestructorAction(
                [&]
                {
                    ++destroyed;
                    ui.registerCallbackFunc( 99, []( const OpButtonAction & ) {} );
                } ) );
            ui.registerCallbackFunc( 1, [owner]( const OpButtonAction & ) {} );
            owner.reset();
            switch( mode )
            {
            case 0:
                ui.removeCallback( 1 );
                break;
            case 1:
                ui.registerCallback( 1, nullptr );
                break;
            case 2:
                ui.registerCallbackFunc( 1, OpButtonWidgetCallbackFunc() );
                break;
            case 3:
                ui.registerCallback( 1, &raw );
                break;
            case 4:
                ui.registerCallbackFunc( 1, []( const OpButtonAction & ) {} );
                break;
            }
            check( destroyed == 1, "old handler was not released after registry change" );
            check( ui.dispatchAction( actionFor( 99 ) ), "destructor could not reenter registry" );
            ui.removeCallback( 1 );
            ui.removeCallback( 99 );
        }
    }

    void rawCallbackReentry()
    {
        OpButtonUI ui;
        RawCallback raw;
        bool called = false;
        raw.callback = [&]( const OpButtonAction & action )
        {
            ui.removeCallback( action.widgetId() );
            called = true;
        };
        ui.registerCallback( 1, &raw );
        check( ui.dispatchAction( actionFor( 1 ) ), "raw callback was not dispatched" );
        check( called && !ui.dispatchAction( actionFor( 1 ) ), "raw callback could not unregister" );
    }

    void exceptionUnwinds()
    {
        OpButtonUI ui;
        ui.registerCallbackFunc( 1, []( const OpButtonAction & )
                                 { throw std::runtime_error( "expected" ); } );
        bool caught = false;
        try
        {
            ui.dispatchAction( actionFor( 1 ) );
        }
        catch( const std::runtime_error & )
        {
            caught = true;
        }
        check( caught, "callback exception was swallowed" );
        ui.removeCallback( 1 );
        ui.registerCallbackFunc( 2, []( const OpButtonAction & ) {} );
        check( ui.dispatchAction( actionFor( 2 ) ), "registry stayed locked after exception" );
    }

    void tabIds()
    {
        OpButtonUI ui;
        for( int index = 0; index < 100; ++index )
        {
            OpButtonString title = opbuttonToString( index );
            auto & tab = ui.addTab( title );
            check( tab.id() == title, "tab did not use the supplied public ID" );
            check( ui.getTab( title ) == &tab, "tab could not be found by its supplied ID" );
        }
    }

    void tabIdentity()
    {
        OpButtonUI ui;
        OpButtonTab & tab = ui.getOrCreateTab( "controls" );
        check( tab.title() == "controls", "default name did not match the ID" );
        int calls = 0;
        OpButtonButton & button = tab.vertical().addButton( "Run" );
        int widgetId = button.internalId();
        button.onClick( OpButtonWidgetCallbackFunc( [&]( const OpButtonAction & ) { ++calls; } ) );
        int revision = ui.getRevision();

        tab.setName( "Game controls" );
        check( ui.getRevision() > revision, "renaming did not invalidate the UI" );
        revision = ui.getRevision();
        check( &ui.getOrCreateTab( "controls" ) == &tab, "lookup replaced the existing tab" );
        check( ui.getRevision() == revision, "lookup changed the UI revision" );
        check( tab.toJson().optString( "title" ) == "Game controls", "display name was not serialized" );
        check( ui.dispatchAction( actionFor( widgetId ) ), "lookup discarded the existing callback" );
        check( calls == 1, "existing callback was not called" );

        ui.getOrCreateTab( "other" ).setName( "Game controls" );
        check( ui.toJson().get( "tabs" ).length() == 2, "equal names merged distinct IDs" );
        ui.addTab( "controls" );
        check( ui.toJson().get( "tabs" ).length() == 2, "replacement matched the display name instead of the ID" );
        check( ui.dispatchAction( actionFor( widgetId ) ) == false, "replacement retained the old callback" );
        ui.removeTab( "controls" );
        check( ui.getTab( "controls" ) == nullptr, "removal did not use the public ID" );
        check( ui.getTab( "other" ) != nullptr, "removal affected a tab with the same name" );
    }

    void concurrentDispatch()
    {
        OpButtonUI ui;
        std::atomic<int> calls{ 0 };
        std::atomic<bool> start{ false };
        ui.registerCallbackFunc( 1, [&]( const OpButtonAction & )
                                 { ++calls; } );
        ui.dispatchAction( actionFor( 1 ) );
        std::vector<std::thread> threads;
        for( int index = 0; index < 2; ++index )
        {
            threads.emplace_back(
                [&]
                {
                    while( !start.load() )
                    {
                        std::this_thread::yield();
                    }
                    for( int attempt = 0; attempt < 1000; ++attempt )
                    {
                        ui.registerCallbackFunc( 1, [&]( const OpButtonAction & )
                                                 { ++calls; } );
                        ui.removeCallback( 1 );
                    }
                } );
            threads.emplace_back(
                [&]
                {
                    while( !start.load() )
                    {
                        std::this_thread::yield();
                    }
                    for( int attempt = 0; attempt < 1000; ++attempt )
                    {
                        ui.dispatchAction( actionFor( 1 ) );
                    }
                } );
        }
        start = true;
        for( auto & thread : threads )
        {
            thread.join();
        }
        check( calls.load() >= 1, "registered callback was never invoked" );
    }

} // namespace

int main( int argc, char ** argv )
{
    try
    {
        check( argc == 2, "expected a test case name" );
        std::string name = argv[1];
        if( name == "remove" )
        {
            removeFromCallback();
        }
        else if( name == "add" )
        {
            addFromCallback();
        }
        else if( name == "replace" )
        {
            replaceFromCallback();
        }
        else if( name == "state" )
        {
            preserveCallbackState();
        }
        else if( name == "lifetime" )
        {
            retainDuringDispatch();
        }
        else if( name == "destroy" )
        {
            reenterFromDestructor();
        }
        else if( name == "raw" )
        {
            rawCallbackReentry();
        }
        else if( name == "exception" )
        {
            exceptionUnwinds();
        }
        else if( name == "ids" )
        {
            tabIds();
        }
        else if( name == "tabs" )
        {
            tabIdentity();
        }
        else if( name == "concurrent" )
        {
            concurrentDispatch();
        }
        else
        {
            throw std::runtime_error( "unknown test case" );
        }
        std::cout << name << " passed\n";
        return 0;
    }
    catch( const std::exception & error )
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
