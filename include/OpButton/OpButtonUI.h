#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonAction.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton {

class OpButtonTab;
class OpButtonWidget;

class OpButtonUI {
public:
    OpButtonUI();
    ~OpButtonUI();

    int nextId();

    // Tab management
    OpButtonTab& addTab(const OpButtonString& title);
    OpButtonTab* getTab(const OpButtonString& id);
    void removeTab(const OpButtonString& id);

    // Widget callbacks
    void registerCallback(int widgetId, OpButtonWidgetCallback* callback);
    void registerCallbackFunc(int widgetId, OpButtonWidgetCallbackFunc func);
    void removeCallback(int widgetId);
    bool dispatchAction(const OpButtonAction& action);

    // Value providers
    void registerProvider(OpButtonWidget* widget);
    void unregisterProvider(OpButtonWidget* widget);
    void pollValues();

    // Revision tracking
    void incrementRevision();
    bool hasUnsent() const;
    void markSent();
    bool hasTabs() const;
    int getRevision() const;

    // Serialization
    OpButtonJson toJson();

private:
    OpButtonVector<OpButtonUniquePtr<OpButtonTab>> m_tabs;
    OpButtonMutex m_tabMutex;

    struct CallbackEntry {
        OpButtonWidgetCallback* raw = nullptr;
        OpButtonWidgetCallbackFunc func;
    };
    OpButtonUnorderedMap<int, CallbackEntry> m_callbacks;
    OpButtonMutex m_callbackMutex;

    OpButtonVector<OpButtonWidget*> m_providers;
    OpButtonMutex m_providerMutex;

    OpButtonAtomic<int> m_idCounter{0};
    OpButtonAtomic<int> m_revision{1};
    int m_sentRevision = 0;
    static const int VERSION = 2;
};

} // namespace OpButton
