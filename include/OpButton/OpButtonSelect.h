#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetActions.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonSelect : public OpButtonWidget
    {
    public:
        OpButtonSelect( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonSelect() override;

        OpButtonSelect & options( const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>> & _opts );
        OpButtonSelect & defaultValue( const OpButtonString & _v );

        OpButtonSelect & onChange( OpButtonWidgetCallback * _callback );
        OpButtonSelect & onChange( OpButtonWidgetCallbackFunc _callback );
        OpButtonSelect & onChange( OpButtonFunction<void( const OpButtonSelectAction & )> _callback );
    };

} // namespace OpButton
