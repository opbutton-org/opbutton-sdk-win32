#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton {

class OpButtonAction;

class OpButtonWidgetCallback {
public:
    virtual ~OpButtonWidgetCallback() = default;
    virtual void onOpButtonWidgetAction(const OpButtonAction& action) = 0;
};

using OpButtonWidgetCallbackFunc = OpButtonFunction<void(const OpButtonAction&)>;

} // namespace OpButton
