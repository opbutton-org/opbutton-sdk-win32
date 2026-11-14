#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton {

class OpButtonTable : public OpButtonWidget {
public:
    OpButtonTable(OpButtonUI& ui, const OpButtonString& label);

    OpButtonTable& columns(const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>>& cols);
    OpButtonTable& rows(const OpButtonVector<OpButtonJson>& rowData);
};

} // namespace OpButton
