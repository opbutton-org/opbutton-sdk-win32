#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonValueProvider.h"

namespace OpButton {

class OpButtonSessionAttributeBuilder {
public:
    OpButtonSessionAttributeBuilder& put(const OpButtonString& key, const OpButtonJson& value);
    OpButtonSessionAttributeBuilder& putProvider(const OpButtonString& key, OpButtonValueProvider* provider);

    const OpButtonMap<OpButtonString, OpButtonJson>& staticAttributes() const;
    const OpButtonMap<OpButtonString, OpButtonValueProvider*>& dynamicAttributes() const;

private:
    OpButtonMap<OpButtonString, OpButtonJson> m_staticAttributes;
    OpButtonMap<OpButtonString, OpButtonValueProvider*> m_dynamicAttributes;
};

} // namespace OpButton
