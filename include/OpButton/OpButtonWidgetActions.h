#pragma once

#include "OpButton/OpButtonAction.h"

namespace OpButton {

class OpButtonSwitchAction {
public:
    explicit OpButtonSwitchAction(const OpButtonAction& action);
    bool value() const;
    const OpButtonAction& action() const;
private:
    OpButtonAction m_action;
};

class OpButtonInputAction {
public:
    explicit OpButtonInputAction(const OpButtonAction& action);
    OpButtonString value() const;
    const OpButtonAction& action() const;
private:
    OpButtonAction m_action;
};

class OpButtonSelectAction {
public:
    explicit OpButtonSelectAction(const OpButtonAction& action);
    OpButtonString value() const;
    const OpButtonAction& action() const;
private:
    OpButtonAction m_action;
};

class OpButtonTextareaAction {
public:
    explicit OpButtonTextareaAction(const OpButtonAction& action);
    OpButtonString value() const;
    const OpButtonAction& action() const;
private:
    OpButtonAction m_action;
};

} // namespace OpButton
