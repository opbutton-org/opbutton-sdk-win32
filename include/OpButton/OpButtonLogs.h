#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton {

class OpButtonLogs : public OpButtonWidget {
public:
    OpButtonLogs(OpButtonUI& ui, const OpButtonString& label);

    OpButtonLogs& maxLines(int v);
};

} // namespace OpButton
