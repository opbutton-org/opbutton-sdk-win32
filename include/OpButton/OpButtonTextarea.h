#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetCallback.h"
#include "OpButton/OpButtonWidgetActions.h"

namespace OpButton {

class OpButtonTextarea : public OpButtonWidget {
public:
    OpButtonTextarea(OpButtonUI& ui, const OpButtonString& label);

    OpButtonTextarea& rows(int r);
    OpButtonTextarea& defaultValue(const OpButtonString& v);

    OpButtonTextarea& onSubmit(OpButtonWidgetCallback* callback);
    OpButtonTextarea& onSubmit(OpButtonWidgetCallbackFunc callback);
    OpButtonTextarea& onSubmit(OpButtonFunction<void(const OpButtonTextareaAction&)> callback);
};

} // namespace OpButton
