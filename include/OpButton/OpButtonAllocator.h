#pragma once

#include <cstddef>

namespace OpButton {

typedef void* (*OpButtonMallocFunc)(size_t size);
typedef void (*OpButtonFreeFunc)(void* ptr);

void opbutton_set_allocator(OpButtonMallocFunc mallocFunc, OpButtonFreeFunc freeFunc);

void* opbutton_malloc(size_t size);
void opbutton_free(void* ptr);

} // namespace OpButton
