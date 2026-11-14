#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetCallback.h"
#include "OpButton/OpButtonWidgetActions.h"

namespace OpButton {

class OpButtonSwitch : public OpButtonWidget {
public:
    OpButtonSwitch(OpButtonUI& ui, const OpButtonString& label);

    OpButtonSwitch& defaultValue(bool v);

    OpButtonSwitch& onChange(OpButtonWidgetCallback* callback);
    OpButtonSwitch& onChange(OpButtonWidgetCallbackFunc callback);
    OpButtonSwitch& onChange(OpButtonFunction<void(const OpButtonSwitchAction&)> callback);
};

} // namespace OpButton
