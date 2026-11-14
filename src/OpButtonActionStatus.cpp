#include "OpButton/OpButtonActionStatus.h"

namespace OpButton {

const char* opbuttonActionStatusValue(OpButtonActionStatus s) {
    switch (s) {
        case OpButtonActionStatus::PENDING: return "pending";
        case OpButtonActionStatus::DELIVERED: return "delivered";
        case OpButtonActionStatus::ACKNOWLEDGED: return "acknowledged";
        case OpButtonActionStatus::UNKNOWN: return "";
    }
    return "";
}

OpButtonActionStatus opbuttonActionStatusFromValue(const OpButtonString& v) {
    if (v == "pending") return OpButtonActionStatus::PENDING;
    if (v == "delivered") return OpButtonActionStatus::DELIVERED;
    if (v == "acknowledged") return OpButtonActionStatus::ACKNOWLEDGED;
    return OpButtonActionStatus::UNKNOWN;
}

} // namespace OpButton
