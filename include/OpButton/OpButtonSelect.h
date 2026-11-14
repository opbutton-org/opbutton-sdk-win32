#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetCallback.h"
#include "OpButton/OpButtonWidgetActions.h"

namespace OpButton {

class OpButtonSelect : public OpButtonWidget {
public:
    OpButtonSelect(OpButtonUI& ui, const OpButtonString& label);

    OpButtonSelect& options(const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>>& opts);
    OpButtonSelect& defaultValue(const OpButtonString& v);

    OpButtonSelect& onChange(OpButtonWidgetCallback* callback);
    OpButtonSelect& onChange(OpButtonWidgetCallbackFunc callback);
    OpButtonSelect& onChange(OpButtonFunction<void(const OpButtonSelectAction&)> callback);
};

} // namespace OpButton
