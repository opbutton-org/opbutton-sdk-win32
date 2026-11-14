#include "OpButton/OpButtonAllocator.h"

#include <cstdlib>

namespace OpButton {

static OpButtonMallocFunc s_mallocFunc = ::malloc;
static OpButtonFreeFunc s_freeFunc = ::free;

void opbutton_set_allocator(OpButtonMallocFunc mallocFunc, OpButtonFreeFunc freeFunc) {
    s_mallocFunc = mallocFunc ? mallocFunc : ::malloc;
    s_freeFunc = freeFunc ? freeFunc : ::free;
}

void* opbutton_malloc(size_t size) {
    return s_mallocFunc(size);
}

void opbutton_free(void* ptr) {
    s_freeFunc(ptr);
}

} // namespace OpButton
