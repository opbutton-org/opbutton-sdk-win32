#pragma once

#include "OpButton/OpButtonAction.h"

namespace OpButton
{
    class OpButtonSliderAction
    {
    public:
        explicit OpButtonSliderAction( const OpButtonAction & _action );
        ~OpButtonSliderAction();

    public:
        double value() const;
        const OpButtonAction & action() const;

    private:
        OpButtonAction m_action;
    };
} // namespace OpButton
