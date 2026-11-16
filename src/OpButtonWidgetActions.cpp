#include "OpButton/OpButtonWidgetActions.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitchAction::OpButtonSwitchAction( const OpButtonAction & _action )
        : m_action( _action )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonSwitchAction::value() const
    {
        return m_action.payload().optBool( "value", false );
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonAction & OpButtonSwitchAction::action() const
    {
        return m_action;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInputAction::OpButtonInputAction( const OpButtonAction & _action )
        : m_action( _action )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonInputAction::value() const
    {
        return m_action.payload().optString( "value" );
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonAction & OpButtonInputAction::action() const
    {
        return m_action;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelectAction::OpButtonSelectAction( const OpButtonAction & _action )
        : m_action( _action )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonSelectAction::value() const
    {
        return m_action.payload().optString( "value" );
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonAction & OpButtonSelectAction::action() const
    {
        return m_action;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextareaAction::OpButtonTextareaAction( const OpButtonAction & _action )
        : m_action( _action )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonTextareaAction::value() const
    {
        return m_action.payload().optString( "value" );
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonAction & OpButtonTextareaAction::action() const
    {
        return m_action;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitchAction::~OpButtonSwitchAction()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInputAction::~OpButtonInputAction()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelectAction::~OpButtonSelectAction()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextareaAction::~OpButtonTextareaAction()
    {
    }
} // namespace OpButton
