#pragma once

namespace OpButton {

enum class OpButtonLogLevel {
    PL_DEBUG,
    PL_INFO,
    PL_WARNING,
    PL_ERROR,
    PL_CRITICAL,
    PL_EXCEPTION
};

const char* opbuttonLogLevelValue(OpButtonLogLevel level);

} // namespace OpButton
