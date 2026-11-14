#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton {

class OpButtonButton : public OpButtonWidget {
public:
    OpButtonButton(OpButtonUI& ui, const OpButtonString& label);

    OpButtonButton& variant(const OpButtonString& v);
    OpButtonButton& color(const OpButtonString& c);
    OpButtonButton& disabled(bool d);

    OpButtonButton& onClick(OpButtonWidgetCallback* callback);
    OpButtonButton& onClick(OpButtonWidgetCallbackFunc callback);
};

} // namespace OpButton
