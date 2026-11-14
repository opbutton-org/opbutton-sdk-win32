#pragma once

#include "OpButton/OpButtonLayout.h"
#include "OpButton/OpButtonJson.h"

namespace OpButton {

class OpButtonUI;

class OpButtonTab {
public:
    OpButtonTab(OpButtonUI& ui, const OpButtonString& title);

    OpButtonTab& setId(const OpButtonString& id) { m_id = id; return *this; }

    OpButtonLayout& vertical();
    OpButtonLayout& horizontal();

    const OpButtonString& title() const { return m_title; }
    const OpButtonString& id() const { return m_id; }
    int internalId() const { return m_internalId; }
    bool hasLayout() const { return m_layout != nullptr; }

    OpButtonJson toJson() const;

private:
    OpButtonUI& m_ui;
    OpButtonString m_title;
    OpButtonString m_id;
    int m_internalId;
    OpButtonUniquePtr<OpButtonLayout> m_layout;
};

} // namespace OpButton
