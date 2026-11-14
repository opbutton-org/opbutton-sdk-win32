#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetCallback.h"
#include "OpButton/OpButtonWidgetActions.h"

namespace OpButton {

class OpButtonInput : public OpButtonWidget {
public:
    OpButtonInput(OpButtonUI& ui, const OpButtonString& label);

    OpButtonInput& inputType(const OpButtonString& t);
    OpButtonInput& defaultValue(const OpButtonString& v);
    OpButtonInput& placeholder(const OpButtonString& p);

    OpButtonInput& onSubmit(OpButtonWidgetCallback* callback);
    OpButtonInput& onSubmit(OpButtonWidgetCallbackFunc callback);
    OpButtonInput& onSubmit(OpButtonFunction<void(const OpButtonInputAction&)> callback);
};

} // namespace OpButton
