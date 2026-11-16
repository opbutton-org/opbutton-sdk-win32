#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton
{
    class OpButtonLogs
        : public OpButtonWidget
    {
    public:
        OpButtonLogs( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonLogs() override;

    public:
        OpButtonLogs & maxLines( int _v );
    };

} // namespace OpButton
