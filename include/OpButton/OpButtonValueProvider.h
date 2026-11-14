#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton {

class OpButtonValueProvider {
public:
    virtual ~OpButtonValueProvider() = default;
    virtual OpButtonString getValue() = 0;
};

} // namespace OpButton
