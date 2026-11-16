#pragma once

#include "OpButton/OpButtonValueProvider.h"

namespace OpButton
{
    class OpButtonLogAttributeBuilder
    {
    public:
        OpButtonLogAttributeBuilder();
        ~OpButtonLogAttributeBuilder();
        OpButtonLogAttributeBuilder & put( const OpButtonString & _key, const OpButtonString & _value );
        OpButtonLogAttributeBuilder & putProvider( const OpButtonString & _key, OpButtonValueProvider * _provider );

        const OpButtonMap<OpButtonString, OpButtonString> & staticAttributes() const;
        const OpButtonMap<OpButtonString, OpButtonValueProvider *> & dynamicAttributes() const;

    private:
        OpButtonMap<OpButtonString, OpButtonString> m_staticAttributes;
        OpButtonMap<OpButtonString, OpButtonValueProvider *> m_dynamicAttributes;
    };

} // namespace OpButton
