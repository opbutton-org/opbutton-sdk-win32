#pragma once

#include "OpButton/OpButtonLayoutChild.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonJsonElement
        : public OpButtonLayoutChild
    {
    public:
        OpButtonJsonElement( OpButtonUI & _ui, const OpButtonJson & _json, int _depth = 0 );
        ~OpButtonJsonElement() override;

    public:
        void attach( const OpButtonJsonCallback & _callback );
        OpButtonJson toJson() const override;

    private:
        OpButtonJson m_json;
        OpButtonString m_identifier;
        OpButtonVector<OpButtonIntrusivePtr<OpButtonJsonElement>> m_children;
    };
}
