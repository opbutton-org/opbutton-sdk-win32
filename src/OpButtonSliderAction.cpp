#include "OpButton/OpButtonSliderAction.h"

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonSliderAction::OpButtonSliderAction( const OpButtonAction & _action )
        : m_action( _action )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    double OpButtonSliderAction::value() const
    {
        return m_action.payload().optDouble( "value" );
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonAction & OpButtonSliderAction::action() const
    {
        return m_action;
    }

    OpButtonSliderAction::~OpButtonSliderAction()
    {
    }
} // namespace OpButton
