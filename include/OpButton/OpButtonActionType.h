#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton {

enum class OpButtonActionType {
    CLICK,
    CHANGE,
    TOGGLE,
    LIVE_START,
    LIVE_UPDATE,
    LIVE_STOP,
    LIVE_TAP,
    LIVE_LONG_PRESS,
    UNKNOWN
};

const char* opbuttonActionTypeValue(OpButtonActionType t);
OpButtonActionType opbuttonActionTypeFromValue(const OpButtonString& v);

} // namespace OpButton
