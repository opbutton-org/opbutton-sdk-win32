#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonValueProvider
    {
    public:
        OpButtonValueProvider();
        virtual ~OpButtonValueProvider();

        virtual OpButtonString getValue() = 0;
    };

} // namespace OpButton
