#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton
{
    class OpButtonStat : public OpButtonWidget
    {
    public:
        OpButtonStat( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonStat() override;

        OpButtonStat & value( const OpButtonString & _v );
        OpButtonStat & unit( const OpButtonString & _u );

        OpButtonStat & valueProvider( OpButtonValueProvider * _provider );
    };

} // namespace OpButton
