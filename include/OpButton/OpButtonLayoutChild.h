#pragma once

#include "OpButton/OpButtonJson.h"

namespace OpButton
{
    class OpButtonLayoutChild : public OpButtonRefCounted
    {
    public:
        OpButtonLayoutChild();
        virtual ~OpButtonLayoutChild() override;

        virtual OpButtonJson toJson() const = 0;
    };

} // namespace OpButton
