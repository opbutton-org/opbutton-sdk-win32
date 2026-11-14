#pragma once

#include "OpButton/OpButtonLogLevel.h"
#include "OpButton/OpButtonTypes.h"

namespace OpButton {

class OpButtonLoggerListener {
public:
    virtual ~OpButtonLoggerListener() = default;
    virtual void onOpButtonLoggerMessage(OpButtonLogLevel level, const OpButtonString& tag, const OpButtonString& message) = 0;
};

} // namespace OpButton
