#include "OpButton/OpButtonActionType.h"

namespace OpButton {

const char* opbuttonActionTypeValue(OpButtonActionType t) {
    switch (t) {
        case OpButtonActionType::CLICK: return "click";
        case OpButtonActionType::CHANGE: return "change";
        case OpButtonActionType::TOGGLE: return "toggle";
        case OpButtonActionType::LIVE_START: return "live_start";
        case OpButtonActionType::LIVE_UPDATE: return "live_update";
        case OpButtonActionType::LIVE_STOP: return "live_stop";
        case OpButtonActionType::LIVE_TAP: return "live_tap";
        case OpButtonActionType::LIVE_LONG_PRESS: return "live_long_press";
        case OpButtonActionType::UNKNOWN: return "";
    }
    return "";
}

OpButtonActionType opbuttonActionTypeFromValue(const OpButtonString& v) {
    if (v == "click") return OpButtonActionType::CLICK;
    if (v == "change") return OpButtonActionType::CHANGE;
    if (v == "toggle") return OpButtonActionType::TOGGLE;
    if (v == "live_start") return OpButtonActionType::LIVE_START;
    if (v == "live_update") return OpButtonActionType::LIVE_UPDATE;
    if (v == "live_stop") return OpButtonActionType::LIVE_STOP;
    if (v == "live_tap") return OpButtonActionType::LIVE_TAP;
    if (v == "live_long_press") return OpButtonActionType::LIVE_LONG_PRESS;
    return OpButtonActionType::UNKNOWN;
}

} // namespace OpButton
