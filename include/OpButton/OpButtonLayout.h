#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonLayoutChild.h"

namespace OpButton
{
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

    class OpButtonLayout : public OpButtonLayoutChild
    {
    public:
        enum class Direction
        {
            VERTICAL,
            HORIZONTAL
        };

        OpButtonLayout( OpButtonUI & _ui, Direction _dir );
        ~OpButtonLayout() override;
        // Sub-layouts
        OpButtonLayout & addVertical();
        OpButtonLayout & addHorizontal();
        OpButtonLayout & addCollapsible( const OpButtonString & _title );

        // Padding
        OpButtonLayout & addPadding( double _weight );

        // Widgets
        OpButtonButton & addButton( const OpButtonString & _label );
        OpButtonLabel & addLabel( const OpButtonString & _text );
        OpButtonStat & addStat( const OpButtonString & _label );
        OpButtonSwitch & addSwitch( const OpButtonString & _label );
        OpButtonInput & addInput( const OpButtonString & _label );
        OpButtonSelect & addSelect( const OpButtonString & _label );
        OpButtonTextarea & addTextarea( const OpButtonString & _label );
        OpButtonTable & addTable( const OpButtonString & _label );
        OpButtonLogs & addLogs( const OpButtonString & _label );

        OpButtonJson toJson() const override;

    private:
        OpButtonUI & m_ui;
        Direction m_direction;
        OpButtonVector<OpButtonIntrusivePtr<OpButtonLayoutChild>> m_children;

        // Internal helpers for padding and collapsible
        class PaddingElement;
        class CollapsibleElement;
    };

} // namespace OpButton
