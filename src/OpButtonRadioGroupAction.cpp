#include "OpButton/OpButtonRadioGroupAction.h"

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonRadioGroupAction::OpButtonRadioGroupAction( const OpButtonAction & _action )
        : m_action( _action )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonString OpButtonRadioGroupAction::value() const
    {
        return m_action.payload().optString( "value" );
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonAction & OpButtonRadioGroupAction::action() const
    {
        return m_action;
    }

    OpButtonRadioGroupAction::~OpButtonRadioGroupAction()
    {
    }
} // namespace OpButton
