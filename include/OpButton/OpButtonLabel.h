#pragma once

#include "OpButton/OpButtonWidget.h"

namespace OpButton
{
    class OpButtonLabel
        : public OpButtonWidget
    {
    public:
        OpButtonLabel( OpButtonUI & _ui, const OpButtonString & _text );
        ~OpButtonLabel() override;

    public:
        OpButtonLabel & text( const OpButtonString & _t );
        OpButtonLabel & color( const OpButtonString & _c );

        OpButtonLabel & textProvider( OpButtonValueProvider * _provider );
    };

} // namespace OpButton
