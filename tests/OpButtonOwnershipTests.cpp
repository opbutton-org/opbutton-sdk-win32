#include "OpButton/OpButtonAllocator.h"
#include "OpButton/OpButtonAllocatorInterface.h"
#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonLabel.h"
#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonUI.h"
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
    using namespace OpButton;

    void check( bool _value, const char * _message )
    {
        if( _value == false )
        {
            throw std::runtime_error( _message );
        }
    }

    class RecordingAllocator : public OpButtonAllocatorInterface
    {
    public:
        RecordingAllocator()
            : allocations( 0 )
            , frees( 0 )
            , fail( false )
        {
        }

        ~RecordingAllocator() override
        {
        }

        void * allocate( size_t _size ) override
        {
            if( fail == true )
            {
                return nullptr;
            }
            void * memory = std::malloc( _size );
            if( memory != nullptr )
            {
                ++allocations;
            }
            return memory;
        }

        void deallocate( void * _memory ) override
        {
            ++frees;
            std::free( _memory );
        }

        int allocations;
        int frees;
        bool fail;
    };

    class Object : public OpButtonRefCounted
    {
    public:
        explicit Object( int & _destroyed )
            : m_destroyed( _destroyed )
        {
        }

        ~Object() override
        {
            ++m_destroyed;
        }

    private:
        int & m_destroyed;
    };

    class Provider : public OpButtonValueProvider
    {
    public:
        Provider()
            : calls( 0 )
        {
        }

        ~Provider() override
        {
        }

        OpButtonString getValue() override
        {
            ++calls;
            if( onValue != nullptr )
            {
                onValue();
            }
            return "value";
        }

        int calls;
        OpButtonFunction<void()> onValue;
    };

    int s_callbackAllocations = 0;
    int s_callbackFrees = 0;

    void * callbackMalloc( size_t _size )
    {
        ++s_callbackAllocations;
        return std::malloc( _size );
    }

    void callbackFree( void * _memory )
    {
        ++s_callbackFrees;
        std::free( _memory );
    }

    void allocatorOwnership()
    {
        RecordingAllocator first;
        RecordingAllocator second;
        opbutton_set_allocator( &first );
        void * memory = opbutton_malloc( 7 );
        check( reinterpret_cast<uintptr_t>( memory ) % alignof( std::max_align_t ) == 0, "allocation alignment" );
        opbutton_set_allocator( &second );
        opbutton_free( memory );
        check( first.allocations == 1 && first.frees == 1, "allocation lost original allocator" );
        check( second.frees == 0, "new allocator freed foreign memory" );
        {
            OpButtonUI ui;
            OpButtonButton & button = ui.addTab( "Allocator" ).horizontal().addButton( "A long label to allocate a string" );
            button.onClick( OpButtonWidgetCallbackFunc( []( const OpButtonAction & ) {} ) );
            OpButtonJson json = ui.toJson();
            check( json.has( "tabs" ), "serialization with external allocator" );
        }
        check( second.allocations > 5, "SDK objects bypassed external allocator" );
        check( second.allocations == second.frees, "SDK allocations survived teardown" );
        opbutton_set_allocator( &callbackMalloc, &callbackFree );
        memory = opbutton_malloc( 0 );
        opbutton_set_allocator( nullptr );
        opbutton_free( memory );
        check( s_callbackAllocations == 1 && s_callbackFrees == 1, "legacy allocator origin not retained" );
        opbutton_free( nullptr );
        bool rejected = false;
        try
        {
            opbutton_set_allocator( &callbackMalloc, nullptr );
        }
        catch( const std::invalid_argument & )
        {
            rejected = true;
        }
        check( rejected, "incomplete allocator pair accepted" );
        second.fail = true;
        opbutton_set_allocator( &second );
        bool failed = false;
        try
        {
            int destroyed = 0;
            OpButtonIntrusivePtr<Object> object( new Object( destroyed ) );
        }
        catch( const std::bad_alloc & )
        {
            failed = true;
        }
        opbutton_set_allocator( nullptr );
        check( failed, "allocation failure not propagated" );
        failed = false;
        try
        {
            OpButtonStlAllocator<uint64_t> allocator;
            allocator.allocate( std::numeric_limits<size_t>::max() );
        }
        catch( const std::bad_alloc & )
        {
            failed = true;
        }
        check( failed, "STL allocation overflow accepted" );
    }

    void intrusiveOwnership()
    {
        int destroyed = 0;
        OpButtonIntrusivePtr<Object> first( new Object( destroyed ) );
        check( first->getReferenceCount() == 1, "initial count" );
        first.reset( first.get() );
        check( first->getReferenceCount() == 1, "reset same object" );
        OpButtonIntrusivePtr<OpButtonRefCounted> base( first );
        check( first->getReferenceCount() == 2, "converting copy count" );
        OpButtonIntrusivePtr<Object> second( std::move( first ) );
        check( first == nullptr, "move source not empty" );
        OpButtonIntrusivePtr<Object> & same = second;
        second = std::move( same );
        check( second != nullptr && second->getReferenceCount() == 2, "self move" );
        base.reset();
        check( second->getReferenceCount() == 1, "base release" );
        second = nullptr;
        check( destroyed == 1, "object not destroyed exactly once" );
    }

    void widgetOwnership()
    {
        OpButtonUI ui;
        Provider provider;
        OpButtonTab & tab = ui.addTab( "First" );
        tab.setId( "first" );
        OpButtonLayout & layout = tab.horizontal();
        layout.addLabel( "Provider" ).textProvider( &provider );
        int id = layout.addButton( "Callback" ).internalId();
        int destroyed = 0;
        OpButtonIntrusivePtr<Object> object( new Object( destroyed ) );
        ui.registerCallbackFunc( id, [object]( const OpButtonAction & ) {} );
        object.reset();
        ui.pollValues();
        check( provider.calls == 1, "provider not registered" );
        ui.removeTab( "first" );
        check( destroyed == 1, "removed tab retained callback" );
        ui.pollValues();
        check( provider.calls == 1, "removed widget remained provider" );
        OpButtonAction action( "a", "s", id, OpButtonActionType::CLICK, OpButtonActionStatus::DELIVERED, OpButtonJson::object() );
        check( ui.dispatchAction( action ) == false, "removed widget remained callback" );
        ui.addTab( "Replace" ).horizontal().addLabel( "Provider" ).textProvider( &provider );
        ui.addTab( "Replace" );
        ui.pollValues();
        check( provider.calls == 1, "replaced tab retained provider" );
        OpButtonTab & reentrant = ui.addTab( "Reentrant" );
        reentrant.setId( "reentrant" );
        reentrant.horizontal().addLabel( "Provider" ).textProvider( &provider );
        provider.onValue = [&ui]()
        { ui.removeTab( "reentrant" ); };
        ui.pollValues();
        check( ui.getTab( "reentrant" ) == nullptr, "provider could not remove its tab" );
        provider.onValue = nullptr;
        ui.pollValues();
        check( provider.calls == 2, "provider retained after removing its own tab" );
    }
} // namespace

int main( int _argc, char ** _argv )
{
    try
    {
        check( _argc == 2, "expected test case" );
        OpButtonString name( _argv[1] );
        if( name == "allocator" )
        {
            allocatorOwnership();
        }
        else if( name == "intrusive" )
        {
            intrusiveOwnership();
        }
        else if( name == "widgets" )
        {
            widgetOwnership();
        }
        else
        {
            throw std::runtime_error( "unknown test case" );
        }
        std::cout << _argv[1] << " passed\n";
        return 0;
    }
    catch( const std::exception & _exception )
    {
        opbutton_set_allocator( nullptr );
        std::cerr << _exception.what() << '\n';
        return 1;
    }
}
