#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonValueProvider.h"

namespace OpButton
{
    class OpButtonSessionAttributeBuilder
    {
    public:
        OpButtonSessionAttributeBuilder();
        ~OpButtonSessionAttributeBuilder();

    public:
        OpButtonSessionAttributeBuilder & put( const OpButtonString & _key, const OpButtonJson & _value );
        OpButtonSessionAttributeBuilder & putProvider( const OpButtonString & _key, OpButtonValueProvider * _provider );

        const OpButtonMap<OpButtonString, OpButtonJson> & staticAttributes() const;
        const OpButtonMap<OpButtonString, OpButtonValueProvider *> & dynamicAttributes() const;

    private:
        OpButtonMap<OpButtonString, OpButtonJson> m_staticAttributes;
        OpButtonMap<OpButtonString, OpButtonValueProvider *> m_dynamicAttributes;
    };

} // namespace OpButton
