#include "OpButton/OpButtonWidgetActions.h"

namespace OpButton {

// ── OpButtonSwitchAction ──

OpButtonSwitchAction::OpButtonSwitchAction(const OpButtonAction& action) : m_action(action) {}
bool OpButtonSwitchAction::value() const { return m_action.payload().optBool("value", false); }
const OpButtonAction& OpButtonSwitchAction::action() const { return m_action; }

// ── OpButtonInputAction ──

OpButtonInputAction::OpButtonInputAction(const OpButtonAction& action) : m_action(action) {}
OpButtonString OpButtonInputAction::value() const { return m_action.payload().optString("value"); }
const OpButtonAction& OpButtonInputAction::action() const { return m_action; }

// ── OpButtonSelectAction ──

OpButtonSelectAction::OpButtonSelectAction(const OpButtonAction& action) : m_action(action) {}
OpButtonString OpButtonSelectAction::value() const { return m_action.payload().optString("value"); }
const OpButtonAction& OpButtonSelectAction::action() const { return m_action; }

// ── OpButtonTextareaAction ──

OpButtonTextareaAction::OpButtonTextareaAction(const OpButtonAction& action) : m_action(action) {}
OpButtonString OpButtonTextareaAction::value() const { return m_action.payload().optString("value"); }
const OpButtonAction& OpButtonTextareaAction::action() const { return m_action; }

} // namespace OpButton
