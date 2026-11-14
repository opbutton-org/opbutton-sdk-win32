#include "OpButton/OpButtonLayout.h"
#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonLabel.h"
#include "OpButton/OpButtonStat.h"
#include "OpButton/OpButtonSwitch.h"
#include "OpButton/OpButtonInput.h"
#include "OpButton/OpButtonSelect.h"
#include "OpButton/OpButtonTextarea.h"
#include "OpButton/OpButtonTable.h"
#include "OpButton/OpButtonLogs.h"

namespace OpButton {

OpButtonLayout::OpButtonLayout(OpButtonUI& ui, Direction dir)
    : m_ui(ui), m_direction(dir) {
}

// ── Sub-layouts ──

OpButtonLayout& OpButtonLayout::addVertical() {
    auto sub = std::make_unique<OpButtonLayout>(m_ui, Direction::VERTICAL);
    auto* ptr = sub.get();
    m_children.push_back(std::move(sub));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonLayout& OpButtonLayout::addHorizontal() {
    auto sub = std::make_unique<OpButtonLayout>(m_ui, Direction::HORIZONTAL);
    auto* ptr = sub.get();
    m_children.push_back(std::move(sub));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonLayout& OpButtonLayout::addCollapsible(const OpButtonString& title) {
    auto collapsible = std::make_unique<CollapsibleElement>(m_ui, title);
    auto& content = collapsible->content();
    m_children.push_back(std::move(collapsible));
    m_ui.incrementRevision();
    return content;
}

// ── Padding ──

OpButtonLayout& OpButtonLayout::addPadding(double weight) {
    m_children.push_back(std::make_unique<PaddingElement>(weight));
    m_ui.incrementRevision();
    return *this;
}

// ── Widgets ──

OpButtonButton& OpButtonLayout::addButton(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonButton>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonLabel& OpButtonLayout::addLabel(const OpButtonString& text) {
    auto w = std::make_unique<OpButtonLabel>(m_ui, text);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonStat& OpButtonLayout::addStat(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonStat>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonSwitch& OpButtonLayout::addSwitch(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonSwitch>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonInput& OpButtonLayout::addInput(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonInput>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonSelect& OpButtonLayout::addSelect(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonSelect>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonTextarea& OpButtonLayout::addTextarea(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonTextarea>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonTable& OpButtonLayout::addTable(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonTable>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

OpButtonLogs& OpButtonLayout::addLogs(const OpButtonString& label) {
    auto w = std::make_unique<OpButtonLogs>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

// ── Serialization ──

OpButtonJson OpButtonLayout::toJson() const {
    OpButtonJson json = OpButtonJson::object();
    json.put("type", OpButtonJson("layout"));
    json.put("direction", OpButtonJson(m_direction == Direction::VERTICAL ? "vertical" : "horizontal"));

    OpButtonJson childrenArr = OpButtonJson::array();
    for (const auto& child : m_children) {
        childrenArr.add(child->toJson());
    }
    json.put("children", childrenArr);
    return json;
}

} // namespace OpButton
