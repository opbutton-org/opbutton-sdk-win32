#pragma once
#include "OpButton/OpButtonRefCounted.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonCallbackFunction
        : public OpButtonRefCounted
    {
    public:
        explicit OpButtonCallbackFunction( OpButtonWidgetCallbackFunc _callback );
        ~OpButtonCallbackFunction() override;

    public:
        void invoke( const OpButtonAction & _action );

    private:
        OpButtonWidgetCallbackFunc m_callback;
    };
} // namespace OpButton
