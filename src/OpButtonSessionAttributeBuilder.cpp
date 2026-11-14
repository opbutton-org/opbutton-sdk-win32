#include "OpButton/OpButtonSessionAttributeBuilder.h"

namespace OpButton {

OpButtonSessionAttributeBuilder& OpButtonSessionAttributeBuilder::put(const OpButtonString& key, const OpButtonJson& value) {
    m_staticAttributes[key] = value;
    return *this;
}

OpButtonSessionAttributeBuilder& OpButtonSessionAttributeBuilder::putProvider(const OpButtonString& key, OpButtonValueProvider* provider) {
    m_dynamicAttributes[key] = provider;
    return *this;
}

const OpButtonMap<OpButtonString, OpButtonJson>& OpButtonSessionAttributeBuilder::staticAttributes() const { return m_staticAttributes; }
const OpButtonMap<OpButtonString, OpButtonValueProvider*>& OpButtonSessionAttributeBuilder::dynamicAttributes() const { return m_dynamicAttributes; }

} // namespace OpButton
