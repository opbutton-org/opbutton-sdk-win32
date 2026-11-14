#pragma once

#include "OpButton/OpButtonValueProvider.h"

namespace OpButton {

class OpButtonLogAttributeBuilder {
public:
    OpButtonLogAttributeBuilder& put(const OpButtonString& key, const OpButtonString& value);
    OpButtonLogAttributeBuilder& putProvider(const OpButtonString& key, OpButtonValueProvider* provider);

    const OpButtonMap<OpButtonString, OpButtonString>& staticAttributes() const;
    const OpButtonMap<OpButtonString, OpButtonValueProvider*>& dynamicAttributes() const;

private:
    OpButtonMap<OpButtonString, OpButtonString> m_staticAttributes;
    OpButtonMap<OpButtonString, OpButtonValueProvider*> m_dynamicAttributes;
};

} // namespace OpButton
