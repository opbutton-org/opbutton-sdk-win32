# Memory ownership

Configure the allocator before initializing the SDK:

    #include "OpButton/OpButtonAllocator.h"
    OpButton::opbutton_set_allocator(&allocateMemory, &deallocateMemory);

Both callbacks are required. Passing two null callbacks restores the CRT allocator.
Alternatively, implement OpButtonAllocatorInterface and pass its address to
opbutton_set_allocator(allocator). Passing a null interface restores the CRT
allocator.

Allocation callbacks and interfaces must be thread-safe, return memory aligned
to std::max_align_t, and remain alive until all their allocations have been
released. Deallocation must not throw. Each allocation retains its original
deallocator, including when the configured allocator changes. Allocation failure
throws std::bad_alloc.

SDK containers and reference-counted objects use this allocator. Allocations
inside standard-library facilities such as std::function and foreign SDKs
remain subject to those libraries' allocation rules.

SDK objects with shared ownership use OpButtonIntrusivePtr and OpButtonRefCounted.
Tabs own their root layout; layouts own their children. References and raw
pointers returned by UI methods are borrowed and expire when their tab or layout
is removed or replaced. The UI must outlive its tabs and widgets. Raw listeners,
widget callbacks and value providers remain caller-owned.

Function callbacks are retained during dispatch and released outside registry
locks. Removing a widget also unregisters its callback and value provider.
LiveKit's required std::shared_ptr types are confined to its private adapter.

Non-template implementations are in .cpp files. Template definitions remain in
headers so client translation units can instantiate them.
