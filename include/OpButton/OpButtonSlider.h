#pragma once

#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonWidgetActions.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonSlider
        : public OpButtonWidget
    {
    public:
        OpButtonSlider( OpButtonUI & _ui, const OpButtonString & _label );
        ~OpButtonSlider() override;

    public:
        OpButtonSlider & range( double _min, double _max, double _step );
        OpButtonSlider & disabled( bool _value );
        OpButtonSlider & unit( const OpButtonString & _value );
        OpButtonSlider & defaultValue( double _v );

        OpButtonSlider & onChange( OpButtonWidgetCallback * _callback );
        OpButtonSlider & onChange( OpButtonWidgetCallbackFunc _callback );
        OpButtonSlider & onChange( OpButtonFunction<void( const OpButtonSliderAction & )> _callback );
    };

} // namespace OpButton
