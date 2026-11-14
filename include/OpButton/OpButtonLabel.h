#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton {

class OpButtonLabel : public OpButtonWidget {
public:
    OpButtonLabel(OpButtonUI& ui, const OpButtonString& text);

    OpButtonLabel& text(const OpButtonString& t);
    OpButtonLabel& color(const OpButtonString& c);

    OpButtonLabel& textProvider(OpButtonValueProvider* provider);
};

} // namespace OpButton
