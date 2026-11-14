#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonLog.h"

namespace OpButton {

OpButtonWidget::OpButtonWidget(OpButtonUI& ui, const OpButtonString& type)
    : m_ui(ui)
    , m_internalId(ui.nextId())
    , m_publicId(type + OpButtonString("-") + opbuttonToString(m_internalId))
    , m_type(type)
    , m_json(OpButtonJson::object()) {
    m_json.put("type", OpButtonJson(type));
    m_json.put("id", OpButtonJson(m_internalId));
}

int OpButtonWidget::internalId() const {
    return m_internalId;
}

const OpButtonString& OpButtonWidget::publicId() const {
    return m_publicId;
}

void OpButtonWidget::setId(const OpButtonString& id) {
    m_publicId = id;
}

const OpButtonString& OpButtonWidget::widgetType() const {
    return m_type;
}

OpButtonJson OpButtonWidget::toJson() const {
    return m_json;
}

void OpButtonWidget::put(const OpButtonString& key, const OpButtonJson& value) {
    m_json.put(key, value);
    m_ui.incrementRevision();
}

void OpButtonWidget::setProvider(const OpButtonString& key, OpButtonValueProvider* provider) {
    m_providerKey = key;
    m_provider = provider;
    if (provider) {
        m_ui.registerProvider(this);
    } else {
        m_ui.unregisterProvider(this);
    }
}

bool OpButtonWidget::pollProvider() {
    if (!m_provider) return false;
    try {
        OpButtonString newValue = m_provider->getValue();
        if (newValue != m_cachedValue) {
            m_cachedValue = newValue;
            if (!m_providerKey.empty()) {
                m_json.put(m_providerKey, OpButtonJson(newValue));
            }
            return true;
        }
    } catch (...) {
        OpButtonLog::e("Value provider failed for widget %d", m_internalId);
    }
    return false;
}

} // namespace OpButton
