#pragma once

#include "OpButton/OpButtonAllocator.h"

#include <cstddef>

namespace OpButton {

template<class T>
class OpButtonStlAllocator {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    OpButtonStlAllocator() noexcept = default;

    template<class U>
    OpButtonStlAllocator(const OpButtonStlAllocator<U>&) noexcept {}

    T* allocate(size_type n) {
        return static_cast<T*>(opbutton_malloc(n * sizeof(T)));
    }

    void deallocate(T* p, size_type) noexcept {
        opbutton_free(p);
    }

    template<class U>
    bool operator==(const OpButtonStlAllocator<U>&) const noexcept { return true; }

    template<class U>
    bool operator!=(const OpButtonStlAllocator<U>&) const noexcept { return false; }
};

} // namespace OpButton
