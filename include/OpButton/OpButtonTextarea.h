#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetActions.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonTextarea
        : public OpButtonWidget
    {
    public:
        OpButtonTextarea( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonTextarea() override;

    public:
        OpButtonTextarea & rows( int _r );
        OpButtonTextarea & defaultValue( const OpButtonString & _v );

        OpButtonTextarea & onSubmit( OpButtonWidgetCallback * _callback );
        OpButtonTextarea & onSubmit( OpButtonWidgetCallbackFunc _callback );
        OpButtonTextarea & onSubmit( OpButtonFunction<void( const OpButtonTextareaAction & )> _callback );
    };

} // namespace OpButton
