#pragma once

#include "OpButton/OpButtonAllocator.h"
#include <cstddef>

namespace OpButton
{
    template<class T>
    class OpButtonStlAllocator
    {
    public:
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        OpButtonStlAllocator() noexcept;
        ~OpButtonStlAllocator();
        template<class U>
        OpButtonStlAllocator( const OpButtonStlAllocator<U> & _other ) noexcept;

        T * allocate( size_type _count );
        void deallocate( T * _memory, size_type _count ) noexcept;

        template<class U>
        bool operator==( const OpButtonStlAllocator<U> & _other ) const noexcept;

        template<class U>
        bool operator!=( const OpButtonStlAllocator<U> & _other ) const noexcept;
    };
} // namespace OpButton

#include "OpButton/OpButtonStlAllocator.inl"
