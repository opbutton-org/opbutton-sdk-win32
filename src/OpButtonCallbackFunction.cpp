#include "OpButton/OpButtonCallbackFunction.h"
#include <utility>

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonCallbackFunction::OpButtonCallbackFunction( OpButtonWidgetCallbackFunc _callback )
        : m_callback( std::move( _callback ) )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonCallbackFunction::~OpButtonCallbackFunction()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonCallbackFunction::invoke( const OpButtonAction & _action )
    {
        m_callback( _action );
    }
} // namespace OpButton
