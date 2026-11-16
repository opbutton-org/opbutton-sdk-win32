#pragma once

#include "OpButton/OpButtonAction.h"

namespace OpButton
{
    class OpButtonSwitchAction
    {
    public:
        explicit OpButtonSwitchAction( const OpButtonAction & _action );
        ~OpButtonSwitchAction();
        bool value() const;
        const OpButtonAction & action() const;

    private:
        OpButtonAction m_action;
    };
} // namespace OpButton
