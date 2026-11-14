#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonLog.h"

#include <algorithm>

namespace OpButton {

OpButtonUI::OpButtonUI() = default;
OpButtonUI::~OpButtonUI() = default;

int OpButtonUI::nextId() {
    return ++m_idCounter;
}

// ── Tab management ──

OpButtonTab& OpButtonUI::addTab(const OpButtonString& title) {
    std::lock_guard<OpButtonMutex> lock(m_tabMutex);
    // Remove existing tab with same title
    m_tabs.erase(
        std::remove_if(m_tabs.begin(), m_tabs.end(),
            [&title](const OpButtonUniquePtr<OpButtonTab>& t) { return t->title() == title; }),
        m_tabs.end());

    auto tab = std::make_unique<OpButtonTab>(*this, title);
    auto* ptr = tab.get();
    m_tabs.push_back(std::move(tab));
    m_revision++;
    return *ptr;
}

OpButtonTab* OpButtonUI::getTab(const OpButtonString& id) {
    std::lock_guard<OpButtonMutex> lock(m_tabMutex);
    for (auto& t : m_tabs) {
        if (t->id() == id) return t.get();
    }
    return nullptr;
}

void OpButtonUI::removeTab(const OpButtonString& id) {
    std::lock_guard<OpButtonMutex> lock(m_tabMutex);
    m_tabs.erase(
        std::remove_if(m_tabs.begin(), m_tabs.end(),
            [&id](const OpButtonUniquePtr<OpButtonTab>& t) { return t->id() == id; }),
        m_tabs.end());
    m_revision++;
}

// ── Widget callbacks ──

void OpButtonUI::registerCallback(int widgetId, OpButtonWidgetCallback* callback) {
    std::lock_guard<OpButtonMutex> lock(m_callbackMutex);
    if (callback) {
        CallbackEntry entry;
        entry.raw = callback;
        m_callbacks[widgetId] = entry;
    } else {
        m_callbacks.erase(widgetId);
    }
}

void OpButtonUI::registerCallbackFunc(int widgetId, OpButtonWidgetCallbackFunc func) {
    std::lock_guard<OpButtonMutex> lock(m_callbackMutex);
    if (func) {
        CallbackEntry entry;
        entry.func = std::move(func);
        m_callbacks[widgetId] = entry;
    } else {
        m_callbacks.erase(widgetId);
    }
}

void OpButtonUI::removeCallback(int widgetId) {
    std::lock_guard<OpButtonMutex> lock(m_callbackMutex);
    m_callbacks.erase(widgetId);
}

bool OpButtonUI::dispatchAction(const OpButtonAction& action) {
    std::lock_guard<OpButtonMutex> lock(m_callbackMutex);
    auto it = m_callbacks.find(action.widgetId());
    if (it == m_callbacks.end()) return false;

    const auto& entry = it->second;
    if (entry.func) {
        entry.func(action);
    } else if (entry.raw) {
        entry.raw->onOpButtonWidgetAction(action);
    }
    return true;
}

// ── Value providers ──

void OpButtonUI::registerProvider(OpButtonWidget* widget) {
    std::lock_guard<OpButtonMutex> lock(m_providerMutex);
    if (std::find(m_providers.begin(), m_providers.end(), widget) == m_providers.end()) {
        m_providers.push_back(widget);
    }
}

void OpButtonUI::unregisterProvider(OpButtonWidget* widget) {
    std::lock_guard<OpButtonMutex> lock(m_providerMutex);
    m_providers.erase(
        std::remove(m_providers.begin(), m_providers.end(), widget),
        m_providers.end());
}

void OpButtonUI::pollValues() {
    std::lock_guard<OpButtonMutex> lock(m_providerMutex);
    for (auto* w : m_providers) {
        if (w->pollProvider()) {
            m_revision++;
        }
    }
}

// ── Revision tracking ──

void OpButtonUI::incrementRevision() {
    m_revision++;
}

bool OpButtonUI::hasUnsent() const {
    return m_revision.load() != m_sentRevision;
}

void OpButtonUI::markSent() {
    m_sentRevision = m_revision.load();
}

bool OpButtonUI::hasTabs() const {
    return !m_tabs.empty();
}

int OpButtonUI::getRevision() const {
    return m_revision.load();
}

// ── Serialization ──

OpButtonJson OpButtonUI::toJson() {
    std::lock_guard<OpButtonMutex> lock(m_tabMutex);
    OpButtonJson json = OpButtonJson::object();
    json.put("version", OpButtonJson(VERSION));
    json.put("revision", OpButtonJson(m_revision.load()));

    OpButtonJson tabsArr = OpButtonJson::array();
    for (const auto& tab : m_tabs) {
        tabsArr.add(tab->toJson());
    }
    json.put("tabs", tabsArr);
    return json;
}

} // namespace OpButton
