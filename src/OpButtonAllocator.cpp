#include "OpButton/OpButtonAllocator.h"
#include "OpButton/OpButtonAllocatorInterface.h"
#include <cstdlib>
#include <limits>
#include <mutex>
#include <new>
#include <stdexcept>

namespace OpButton
{
    namespace Detail
    {
        struct alignas( std::max_align_t ) AllocationDesc
        {

        public:
            AllocationDesc( OpButtonFreeFunc _free, OpButtonAllocatorInterface * _allocator );
            ~AllocationDesc();

        public:
            OpButtonFreeFunc freeFunc;
            OpButtonAllocatorInterface * allocator;
        };

        //////////////////////////////////////////////////////////////////////////
        AllocationDesc::AllocationDesc( OpButtonFreeFunc _free, OpButtonAllocatorInterface * _allocator )
            : freeFunc( _free )
            , allocator( _allocator )
        {
        }

        //////////////////////////////////////////////////////////////////////////
        AllocationDesc::~AllocationDesc()
        {
        }

        static std::mutex s_allocatorMutex;
        static OpButtonMallocFunc s_mallocFunc = &std::malloc;
        static OpButtonFreeFunc s_freeFunc = &std::free;
        static OpButtonAllocatorInterface * s_allocator = nullptr;
    } // namespace Detail

    //////////////////////////////////////////////////////////////////////////
    void opbutton_set_allocator( OpButtonMallocFunc _malloc, OpButtonFreeFunc _free )
    {
        if( ( _malloc == nullptr ) != ( _free == nullptr ) )
        {
            throw std::invalid_argument( "OpButton allocator requires both allocation "
                                         "and deallocation callbacks" );
        }
        std::lock_guard<std::mutex> guard( Detail::s_allocatorMutex );
        Detail::s_mallocFunc = _malloc != nullptr ? _malloc : &std::malloc;
        Detail::s_freeFunc = _free != nullptr ? _free : &std::free;
        Detail::s_allocator = nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    void opbutton_set_allocator( OpButtonAllocatorInterface * _allocator )
    {
        std::lock_guard<std::mutex> guard( Detail::s_allocatorMutex );
        Detail::s_allocator = _allocator;
        Detail::s_mallocFunc = &std::malloc;
        Detail::s_freeFunc = &std::free;
    }

    //////////////////////////////////////////////////////////////////////////
    void * opbutton_malloc( size_t _size )
    {
        if( _size > std::numeric_limits<size_t>::max() - sizeof( Detail::AllocationDesc ) )
        {
            throw std::bad_alloc();
        }
        OpButtonMallocFunc mallocFunc;
        OpButtonFreeFunc freeFunc;
        OpButtonAllocatorInterface * allocator;
        {
            std::lock_guard<std::mutex> guard( Detail::s_allocatorMutex );
            mallocFunc = Detail::s_mallocFunc;
            freeFunc = Detail::s_freeFunc;
            allocator = Detail::s_allocator;
        }
        size_t allocationSize = sizeof( Detail::AllocationDesc ) + _size;
        void * memory = allocator != nullptr ? allocator->allocate( allocationSize ) : mallocFunc( allocationSize );
        if( memory == nullptr )
        {
            throw std::bad_alloc();
        }
        Detail::AllocationDesc * allocation = new( memory ) Detail::AllocationDesc( freeFunc, allocator );
        void * result = allocation + 1;
        return result;
    }

    //////////////////////////////////////////////////////////////////////////
    void opbutton_free( void * _memory )
    {
        if( _memory == nullptr )
        {
            return;
        }
        Detail::AllocationDesc * allocation = static_cast<Detail::AllocationDesc *>( _memory ) - 1;
        OpButtonFreeFunc freeFunc = allocation->freeFunc;
        OpButtonAllocatorInterface * allocator = allocation->allocator;
        allocation->~AllocationDesc();
        if( allocator != nullptr )
        {
            allocator->deallocate( allocation );
            return;
        }
        freeFunc( allocation );
    }
} // namespace OpButton
