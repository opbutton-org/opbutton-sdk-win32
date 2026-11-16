#include "OpButton/OpButtonRefCounted.h"
#include "OpButton/OpButtonAllocator.h"
#include <cassert>

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonRefCounted::OpButtonRefCounted()
        : m_references( 0 )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonRefCounted::~OpButtonRefCounted()
    {
        assert( m_references.load( std::memory_order_relaxed ) == 0 );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonRefCounted::addRef() const noexcept
    {
        m_references.fetch_add( 1, std::memory_order_relaxed );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonRefCounted::release() const noexcept
    {
        uint32_t references = m_references.fetch_sub( 1, std::memory_order_acq_rel );
        assert( references != 0 );
        if( references != 1 )
        {
            return;
        }
        delete this;
    }

    //////////////////////////////////////////////////////////////////////////
    uint32_t OpButtonRefCounted::getReferenceCount() const noexcept
    {
        uint32_t references = m_references.load( std::memory_order_relaxed );
        return references;
    }

    //////////////////////////////////////////////////////////////////////////
    void * OpButtonRefCounted::operator new( size_t _size )
    {
        void * memory = opbutton_malloc( _size );
        return memory;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonRefCounted::operator delete( void * _memory ) noexcept
    {
        opbutton_free( _memory );
    }
} // namespace OpButton
