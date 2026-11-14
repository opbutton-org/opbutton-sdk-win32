#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton {

class OpButtonStat : public OpButtonWidget {
public:
    OpButtonStat(OpButtonUI& ui, const OpButtonString& label);

    OpButtonStat& value(const OpButtonString& v);
    OpButtonStat& unit(const OpButtonString& u);

    OpButtonStat& valueProvider(OpButtonValueProvider* provider);
};

} // namespace OpButton
