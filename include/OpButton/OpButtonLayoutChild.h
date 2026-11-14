#pragma once

#include "OpButton/OpButtonJson.h"

namespace OpButton {

class OpButtonLayoutChild {
public:
    virtual ~OpButtonLayoutChild() = default;
    virtual OpButtonJson toJson() const = 0;
};

} // namespace OpButton
