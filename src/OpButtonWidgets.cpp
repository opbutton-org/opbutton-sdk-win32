#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonSwitch.h"
#include "OpButton/OpButtonInput.h"
#include "OpButton/OpButtonSelect.h"
#include "OpButton/OpButtonTextarea.h"
#include "OpButton/OpButtonLabel.h"
#include "OpButton/OpButtonStat.h"
#include "OpButton/OpButtonTable.h"
#include "OpButton/OpButtonLogs.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton {

// ── OpButtonButton ──

OpButtonButton::OpButtonButton(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "button") {
    put("label", OpButtonJson(label));
}

OpButtonButton& OpButtonButton::variant(const OpButtonString& v) { put("variant", OpButtonJson(v)); return *this; }
OpButtonButton& OpButtonButton::color(const OpButtonString& c) { put("color", OpButtonJson(c)); return *this; }
OpButtonButton& OpButtonButton::disabled(bool d) { put("disabled", OpButtonJson(d)); return *this; }

OpButtonButton& OpButtonButton::onClick(OpButtonWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

OpButtonButton& OpButtonButton::onClick(OpButtonWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

// ── OpButtonSwitch ──

OpButtonSwitch::OpButtonSwitch(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "switch") {
    put("label", OpButtonJson(label));
}

OpButtonSwitch& OpButtonSwitch::defaultValue(bool v) { put("defaultValue", OpButtonJson(v)); return *this; }

OpButtonSwitch& OpButtonSwitch::onChange(OpButtonWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

OpButtonSwitch& OpButtonSwitch::onChange(OpButtonWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

OpButtonSwitch& OpButtonSwitch::onChange(OpButtonFunction<void(const OpButtonSwitchAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const OpButtonAction& action) {
        cb(OpButtonSwitchAction(action));
    });
    return *this;
}

// ── OpButtonInput ──

OpButtonInput::OpButtonInput(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "input") {
    put("label", OpButtonJson(label));
}

OpButtonInput& OpButtonInput::inputType(const OpButtonString& t) { put("inputType", OpButtonJson(t)); return *this; }
OpButtonInput& OpButtonInput::defaultValue(const OpButtonString& v) { put("defaultValue", OpButtonJson(v)); return *this; }
OpButtonInput& OpButtonInput::placeholder(const OpButtonString& p) { put("placeholder", OpButtonJson(p)); return *this; }

OpButtonInput& OpButtonInput::onSubmit(OpButtonWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

OpButtonInput& OpButtonInput::onSubmit(OpButtonWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

OpButtonInput& OpButtonInput::onSubmit(OpButtonFunction<void(const OpButtonInputAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const OpButtonAction& action) {
        cb(OpButtonInputAction(action));
    });
    return *this;
}

// ── OpButtonSelect ──

OpButtonSelect::OpButtonSelect(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "select") {
    put("label", OpButtonJson(label));
}

OpButtonSelect& OpButtonSelect::options(const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>>& opts) {
    OpButtonJson arr = OpButtonJson::array();
    for (const auto& opt : opts) {
        arr.add(OpButtonJson::object()
            .put("value", OpButtonJson(opt.first))
            .put("label", OpButtonJson(opt.second)));
    }
    put("options", arr);
    return *this;
}

OpButtonSelect& OpButtonSelect::defaultValue(const OpButtonString& v) { put("defaultValue", OpButtonJson(v)); return *this; }

OpButtonSelect& OpButtonSelect::onChange(OpButtonWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

OpButtonSelect& OpButtonSelect::onChange(OpButtonWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

OpButtonSelect& OpButtonSelect::onChange(OpButtonFunction<void(const OpButtonSelectAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const OpButtonAction& action) {
        cb(OpButtonSelectAction(action));
    });
    return *this;
}

// ── OpButtonTextarea ──

OpButtonTextarea::OpButtonTextarea(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "textarea") {
    put("label", OpButtonJson(label));
}

OpButtonTextarea& OpButtonTextarea::rows(int r) { put("rows", OpButtonJson(r)); return *this; }
OpButtonTextarea& OpButtonTextarea::defaultValue(const OpButtonString& v) { put("defaultValue", OpButtonJson(v)); return *this; }

OpButtonTextarea& OpButtonTextarea::onSubmit(OpButtonWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

OpButtonTextarea& OpButtonTextarea::onSubmit(OpButtonWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

OpButtonTextarea& OpButtonTextarea::onSubmit(OpButtonFunction<void(const OpButtonTextareaAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const OpButtonAction& action) {
        cb(OpButtonTextareaAction(action));
    });
    return *this;
}

// ── OpButtonLabel ──

OpButtonLabel::OpButtonLabel(OpButtonUI& ui, const OpButtonString& text)
    : OpButtonWidget(ui, "label") {
    put("text", OpButtonJson(text));
}

OpButtonLabel& OpButtonLabel::text(const OpButtonString& t) { put("text", OpButtonJson(t)); return *this; }
OpButtonLabel& OpButtonLabel::color(const OpButtonString& c) { put("color", OpButtonJson(c)); return *this; }

OpButtonLabel& OpButtonLabel::textProvider(OpButtonValueProvider* provider) {
    setProvider("text", provider);
    return *this;
}

// ── OpButtonStat ──

OpButtonStat::OpButtonStat(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "stat") {
    put("label", OpButtonJson(label));
}

OpButtonStat& OpButtonStat::value(const OpButtonString& v) { put("value", OpButtonJson(v)); return *this; }
OpButtonStat& OpButtonStat::unit(const OpButtonString& u) { put("unit", OpButtonJson(u)); return *this; }

OpButtonStat& OpButtonStat::valueProvider(OpButtonValueProvider* provider) {
    setProvider("value", provider);
    return *this;
}

// ── OpButtonTable ──

OpButtonTable::OpButtonTable(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "table") {
    put("label", OpButtonJson(label));
}

OpButtonTable& OpButtonTable::columns(const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>>& cols) {
    OpButtonJson arr = OpButtonJson::array();
    for (const auto& col : cols) {
        arr.add(OpButtonJson::object()
            .put("key", OpButtonJson(col.first))
            .put("label", OpButtonJson(col.second)));
    }
    put("columns", arr);
    return *this;
}

OpButtonTable& OpButtonTable::rows(const OpButtonVector<OpButtonJson>& rowData) {
    OpButtonJson arr = OpButtonJson::array();
    for (const auto& row : rowData) {
        arr.add(row);
    }
    put("rows", arr);
    return *this;
}

// ── OpButtonLogs ──

OpButtonLogs::OpButtonLogs(OpButtonUI& ui, const OpButtonString& label)
    : OpButtonWidget(ui, "logs") {
    put("label", OpButtonJson(label));
}

OpButtonLogs& OpButtonLogs::maxLines(int v) { put("maxLines", OpButtonJson(v)); return *this; }

} // namespace OpButton
