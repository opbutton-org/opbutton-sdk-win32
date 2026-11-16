#pragma once

#include "OpButton/OpButtonJson.h"

namespace OpButton
{
    class OpButtonUI;

    class OpButtonLayoutChild
        : public OpButtonRefCounted
    {
    public:
        explicit OpButtonLayoutChild( OpButtonUI & _ui );
        int internalId() const;
        OpButtonLayoutChild & width( double _pixels );
        OpButtonLayoutChild & contentWidth();
        OpButtonLayoutChild & fill( double _weight = 1.0 );
        OpButtonLayoutChild & minWidth( double _pixels );
        OpButtonLayoutChild & maxWidth( double _pixels );
        OpButtonLayoutChild & alignSelf( const OpButtonString & _value );
        virtual ~OpButtonLayoutChild() override;

    public:
        virtual OpButtonJson toJson() const = 0;

    protected:
        static double dimension_( double _value, bool _positive = false );
        static const OpButtonString & alignment_( const OpButtonString & _value );
        OpButtonJson decorate_( OpButtonJson _json ) const;
        OpButtonUI & m_ui;

    private:
        OpButtonLayoutChild & place_( const OpButtonString & _key, const OpButtonJson & _value );
        int m_elementId;
        OpButtonJson m_placement;
    };

} // namespace OpButton
