#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetActions.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonSwitch : public OpButtonWidget
    {
    public:
        OpButtonSwitch( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonSwitch() override;

        OpButtonSwitch & defaultValue( bool _v );

        OpButtonSwitch & onChange( OpButtonWidgetCallback * _callback );
        OpButtonSwitch & onChange( OpButtonWidgetCallbackFunc _callback );
        OpButtonSwitch & onChange( OpButtonFunction<void( const OpButtonSwitchAction & )> _callback );
    };

} // namespace OpButton
