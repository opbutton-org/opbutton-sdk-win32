#pragma once

#include "OpButton/OpButtonAction.h"

namespace OpButton
{
    class OpButtonRadioGroupAction
    {
    public:
        explicit OpButtonRadioGroupAction( const OpButtonAction & _action );
        ~OpButtonRadioGroupAction();

    public:
        OpButtonString value() const;
        const OpButtonAction & action() const;

    private:
        OpButtonAction m_action;
    };
} // namespace OpButton
