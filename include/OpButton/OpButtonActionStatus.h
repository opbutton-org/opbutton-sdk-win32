#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton {

enum class OpButtonActionStatus {
    PENDING,
    DELIVERED,
    ACKNOWLEDGED,
    UNKNOWN
};

const char* opbuttonActionStatusValue(OpButtonActionStatus s);
OpButtonActionStatus opbuttonActionStatusFromValue(const OpButtonString& v);

} // namespace OpButton
