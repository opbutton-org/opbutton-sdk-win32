#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetActions.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonInput
        : public OpButtonWidget
    {
    public:
        OpButtonInput( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonInput() override;

    public:
        OpButtonInput & inputType( const OpButtonString & _t );
        OpButtonInput & defaultValue( const OpButtonString & _v );
        OpButtonInput & placeholder( const OpButtonString & _p );

        OpButtonInput & onSubmit( OpButtonWidgetCallback * _callback );
        OpButtonInput & onSubmit( OpButtonWidgetCallbackFunc _callback );
        OpButtonInput & onSubmit( OpButtonFunction<void( const OpButtonInputAction & )> _callback );
    };

} // namespace OpButton
