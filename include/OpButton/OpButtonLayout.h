#pragma once

#include "OpButton/OpButtonLayoutChild.h"
#include "OpButton/OpButtonJson.h"

namespace OpButton {

class OpButtonUI;
class OpButtonButton;
class OpButtonLabel;
class OpButtonStat;
class OpButtonSwitch;
class OpButtonInput;
class OpButtonSelect;
class OpButtonTextarea;
class OpButtonTable;
class OpButtonLogs;

class OpButtonLayout : public OpButtonLayoutChild {
public:
    enum class Direction { VERTICAL, HORIZONTAL };

    OpButtonLayout(OpButtonUI& ui, Direction dir);
    ~OpButtonLayout() override = default;

    // Sub-layouts
    OpButtonLayout& addVertical();
    OpButtonLayout& addHorizontal();
    OpButtonLayout& addCollapsible(const OpButtonString& title);

    // Padding
    OpButtonLayout& addPadding(double weight);

    // Widgets
    OpButtonButton& addButton(const OpButtonString& label);
    OpButtonLabel& addLabel(const OpButtonString& text);
    OpButtonStat& addStat(const OpButtonString& label);
    OpButtonSwitch& addSwitch(const OpButtonString& label);
    OpButtonInput& addInput(const OpButtonString& label);
    OpButtonSelect& addSelect(const OpButtonString& label);
    OpButtonTextarea& addTextarea(const OpButtonString& label);
    OpButtonTable& addTable(const OpButtonString& label);
    OpButtonLogs& addLogs(const OpButtonString& label);

    OpButtonJson toJson() const override;

private:
    OpButtonUI& m_ui;
    Direction m_direction;
    OpButtonVector<OpButtonUniquePtr<OpButtonLayoutChild>> m_children;

    // Internal helpers for padding and collapsible
    class PaddingElement : public OpButtonLayoutChild {
    public:
        PaddingElement(double weight) : m_weight(weight) {}
        OpButtonJson toJson() const override {
            return OpButtonJson::object()
                .put("type", OpButtonJson("padding"))
                .put("weight", OpButtonJson(m_weight));
        }
    private:
        double m_weight;
    };

    class CollapsibleElement : public OpButtonLayoutChild {
    public:
        CollapsibleElement(OpButtonUI& ui, const OpButtonString& title)
            : m_title(title), m_content(OpButtonUniquePtr<OpButtonLayout>(new OpButtonLayout(ui, Direction::VERTICAL))) {}
        OpButtonLayout& content() { return *m_content; }
        OpButtonJson toJson() const override {
            OpButtonJson contentJson = m_content->toJson();
            return OpButtonJson::object()
                .put("type", OpButtonJson("collapsible"))
                .put("title", OpButtonJson(m_title))
                .put("children", contentJson.optArray("children"));
        }
    private:
        OpButtonString m_title;
        OpButtonUniquePtr<OpButtonLayout> m_content;
    };
};

} // namespace OpButton
