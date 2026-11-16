#pragma once

namespace OpButton
{
    class OpButtonAction;

    class OpButtonActionListener
    {
    public:
        OpButtonActionListener();
        virtual ~OpButtonActionListener();

    public:
        virtual void onOpButtonActionReceived( const OpButtonAction & _action ) = 0;
    };

} // namespace OpButton
