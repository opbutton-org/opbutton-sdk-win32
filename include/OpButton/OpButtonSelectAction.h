#pragma once

#include "OpButton/OpButtonAction.h"

namespace OpButton
{
    class OpButtonSelectAction
    {
    public:
        explicit OpButtonSelectAction( const OpButtonAction & _action );
        ~OpButtonSelectAction();

    public:
        OpButtonString value() const;
        const OpButtonAction & action() const;

    private:
        OpButtonAction m_action;
    };
} // namespace OpButton
