#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonButton
        : public OpButtonWidget
    {
    public:
        OpButtonButton( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonButton() override;

    public:
        OpButtonButton & variant( const OpButtonString & _v );
        OpButtonButton & color( const OpButtonString & _c );
        OpButtonButton & disabled( bool _d );

        OpButtonButton & onClick( OpButtonWidgetCallback * _callback );
        OpButtonButton & onClick( OpButtonWidgetCallbackFunc _callback );
    };

} // namespace OpButton
