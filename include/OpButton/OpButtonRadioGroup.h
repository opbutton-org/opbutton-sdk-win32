#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetActions.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonRadioGroup
        : public OpButtonWidget
    {
    public:
        OpButtonRadioGroup( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonRadioGroup() override;

    public:
        OpButtonRadioGroup & options( const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>> & _opts );
        OpButtonRadioGroup & disabled( bool _value );
        OpButtonRadioGroup & optionDirection( const OpButtonString & _value );
        OpButtonRadioGroup & defaultValue( const OpButtonString & _v );

        OpButtonRadioGroup & onChange( OpButtonWidgetCallback * _callback );
        OpButtonRadioGroup & onChange( OpButtonWidgetCallbackFunc _callback );
        OpButtonRadioGroup & onChange( OpButtonFunction<void( const OpButtonRadioGroupAction & )> _callback );
    };

} // namespace OpButton
