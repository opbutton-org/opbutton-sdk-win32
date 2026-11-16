#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonAction;

    class OpButtonWidgetCallback
    {
    public:
        OpButtonWidgetCallback();
        virtual ~OpButtonWidgetCallback();

        virtual void onOpButtonWidgetAction( const OpButtonAction & _action ) = 0;
    };

    using OpButtonWidgetCallbackFunc = OpButtonFunction<void( const OpButtonAction & )>;

} // namespace OpButton
