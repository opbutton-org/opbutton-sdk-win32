#pragma once
#include <cstddef>

namespace OpButton
{
    class OpButtonAllocatorInterface;
    typedef void * ( *OpButtonMallocFunc )( size_t _size );
    typedef void ( *OpButtonFreeFunc )( void * _memory );
    // Both callbacks must be supplied together. Passing two null callbacks restores
    // the process allocator. Allocations retain the deallocator that created them.
    void opbutton_set_allocator( OpButtonMallocFunc _malloc, OpButtonFreeFunc _free );
    // Allocators must be thread-safe and return memory aligned to std::max_align_t.
    // Deallocation must not throw. The allocator must outlive its allocations.
    // Passing null restores the process allocator for subsequent allocations.
    void opbutton_set_allocator( OpButtonAllocatorInterface * _allocator );
    void * opbutton_malloc( size_t _size );
    void opbutton_free( void * _memory );
} // namespace OpButton
