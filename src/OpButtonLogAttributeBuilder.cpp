#include "OpButton/OpButtonLogAttributeBuilder.h"

namespace OpButton {

OpButtonLogAttributeBuilder& OpButtonLogAttributeBuilder::put(const OpButtonString& key, const OpButtonString& value) {
    m_staticAttributes[key] = value;
    return *this;
}

OpButtonLogAttributeBuilder& OpButtonLogAttributeBuilder::putProvider(const OpButtonString& key, OpButtonValueProvider* provider) {
    m_dynamicAttributes[key] = provider;
    return *this;
}

const OpButtonMap<OpButtonString, OpButtonString>& OpButtonLogAttributeBuilder::staticAttributes() const { return m_staticAttributes; }
const OpButtonMap<OpButtonString, OpButtonValueProvider*>& OpButtonLogAttributeBuilder::dynamicAttributes() const { return m_dynamicAttributes; }

} // namespace OpButton
