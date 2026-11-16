#pragma once

#include <limits>
#include <new>

namespace OpButton
{
    template<class T>
    OpButtonStlAllocator<T>::OpButtonStlAllocator() noexcept
    {
    }

    template<class T>
    OpButtonStlAllocator<T>::~OpButtonStlAllocator()
    {
    }

    template<class T>
    template<class U>
    OpButtonStlAllocator<T>::OpButtonStlAllocator( const OpButtonStlAllocator<U> & _other ) noexcept
    {
        (void)_other;
    }

    template<class T>
    T * OpButtonStlAllocator<T>::allocate( size_type _count )
    {
        static_assert( alignof( T ) <= alignof( std::max_align_t ), "OpButton allocator does not support over-aligned types" );
        if( _count > std::numeric_limits<size_type>::max() / sizeof( T ) )
        {
            throw std::bad_array_new_length();
        }
        size_type size = _count * sizeof( T );
        void * memory = opbutton_malloc( size );
        return static_cast<T *>( memory );
    }

    template<class T>
    void OpButtonStlAllocator<T>::deallocate( T * _memory, size_type _count ) noexcept
    {
        (void)_count;
        opbutton_free( _memory );
    }

    template<class T>
    template<class U>
    bool OpButtonStlAllocator<T>::operator==( const OpButtonStlAllocator<U> & _other ) const noexcept
    {
        (void)_other;
        return true;
    }

    template<class T>
    template<class U>
    bool OpButtonStlAllocator<T>::operator!=( const OpButtonStlAllocator<U> & _other ) const noexcept
    {
        (void)_other;
        return false;
    }
} // namespace OpButton
