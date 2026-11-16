#pragma once
#include <cstddef>

namespace OpButton
{
    class OpButtonAllocatorInterface
    {
    public:
        OpButtonAllocatorInterface();
        virtual ~OpButtonAllocatorInterface();

    public:
        virtual void * allocate( size_t _size ) = 0;
        virtual void deallocate( void * _memory ) = 0;
    };
} // namespace OpButton
