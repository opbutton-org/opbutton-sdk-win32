#pragma once

#include "OpButton/OpButtonAction.h"

namespace OpButton
{
    class OpButtonTextareaAction
    {
    public:
        explicit OpButtonTextareaAction( const OpButtonAction & _action );
        ~OpButtonTextareaAction();

    public:
        OpButtonString value() const;
        const OpButtonAction & action() const;

    private:
        OpButtonAction m_action;
    };
} // namespace OpButton
