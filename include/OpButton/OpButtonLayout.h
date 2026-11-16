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
    class OpButtonRadioGroup;
    class OpButtonSlider;

    class OpButtonLayout
        : public OpButtonLayoutChild
    {
    public:
        enum class Direction
        {
            VERTICAL,
            HORIZONTAL,
            GRID
        };

    public:
        OpButtonLayout( OpButtonUI & _ui, Direction _dir );
        ~OpButtonLayout() override;

    public:
        // Sub-layouts
        OpButtonLayout & addVertical();
        OpButtonLayout & addHorizontal();
        OpButtonLayout & addGrid();
        OpButtonLayout & addGroup( const OpButtonString & _title );
        OpButtonLayout & gap( double _pixels );
        OpButtonLayout & padding( double _pixels );
        OpButtonLayout & align( const OpButtonString & _value );
        OpButtonLayout & justify( const OpButtonString & _value );
        OpButtonLayout & wrap( bool _value );
        OpButtonLayout & density( const OpButtonString & _value );
        OpButtonLayout & columns( int _value );
        OpButtonLayout & minCellWidth( double _pixels );
        OpButtonLayout & title( const OpButtonString & _value );
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
        OpButtonRadioGroup & addRadioGroup( const OpButtonString & _label );
        OpButtonSlider & addSlider( const OpButtonString & _label );

        OpButtonJson toJson() const override;

    private:
        OpButtonLayout & setting_( const OpButtonString & _key, const OpButtonJson & _value );
        OpButtonJson m_settings;
        Direction m_direction;
        OpButtonVector<OpButtonIntrusivePtr<OpButtonLayoutChild>> m_children;

    private:
        // Internal helpers for padding and collapsible
        class PaddingElement;
        class CollapsibleElement;
    };

} // namespace OpButton
