#pragma once

#include "OpButton/OpButtonTypes.h"

namespace OpButton
{
    class OpButtonValueProvider
    {
    public:
        OpButtonValueProvider();
        virtual ~OpButtonValueProvider();

    public:
        virtual OpButtonString getValue() = 0;
    };

} // namespace OpButton
