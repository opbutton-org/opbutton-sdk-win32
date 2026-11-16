#pragma once

#include "OpButton/OpButtonAction.h"

namespace OpButton
{
    class OpButtonInputAction
    {
    public:
        explicit OpButtonInputAction( const OpButtonAction & _action );
        ~OpButtonInputAction();

    public:
        OpButtonString value() const;
        const OpButtonAction & action() const;

    private:
        OpButtonAction m_action;
    };
} // namespace OpButton
