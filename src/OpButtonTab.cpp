#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton {

OpButtonTab::OpButtonTab(OpButtonUI& ui, const OpButtonString& title)
    : m_ui(ui)
    , m_title(title)
    , m_internalId(ui.nextId())
    , m_id(OpButtonString("tab-") + opbuttonToString(m_internalId)) {
}

OpButtonLayout& OpButtonTab::vertical() {
    m_layout = std::make_unique<OpButtonLayout>(m_ui, OpButtonLayout::Direction::VERTICAL);
    return *m_layout;
}

OpButtonLayout& OpButtonTab::horizontal() {
    m_layout = std::make_unique<OpButtonLayout>(m_ui, OpButtonLayout::Direction::HORIZONTAL);
    return *m_layout;
}

OpButtonJson OpButtonTab::toJson() const {
    OpButtonJson json = OpButtonJson::object();
    json.put("id", OpButtonJson(m_internalId));
    json.put("title", OpButtonJson(m_title));
    if (m_layout) {
        json.put("layout", m_layout->toJson());
    }
    return json;
}

} // namespace OpButton
