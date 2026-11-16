#include "OpButton/OpButtonRadioGroup.h"
#include "OpButton/OpButtonUI.h"
#include <stdexcept>

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonRadioGroup::OpButtonRadioGroup( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "radio" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonRadioGroup & OpButtonRadioGroup::options( const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>> & _opts )
    {
        OpButtonJson arr = OpButtonJson::array();
        for( const auto & opt : _opts )
        {
            arr.add( OpButtonJson::object().put( "value", OpButtonJson( opt.first ) ).put( "label", OpButtonJson( opt.second ) ) );
        }
        this->put_( "options", arr );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonRadioGroup & OpButtonRadioGroup::defaultValue( const OpButtonString & _v )
    {
        this->put_( "defaultValue", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonRadioGroup & OpButtonRadioGroup::onChange( OpButtonWidgetCallback * _callback )
    {
        m_ui.registerCallback( this->internalId(), _callback );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonRadioGroup & OpButtonRadioGroup::onChange( OpButtonWidgetCallbackFunc _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), std::move( _callback ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonRadioGroup & OpButtonRadioGroup::onChange( OpButtonFunction<void( const OpButtonRadioGroupAction & )> _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), [cb = std::move( _callback )]( const OpButtonAction & _action )
                                   { cb( OpButtonRadioGroupAction( _action ) ); } );
        return *this;
    }

    OpButtonRadioGroup & OpButtonRadioGroup::disabled( bool _value )
    {
        this->put_( "disabled", OpButtonJson( _value ) );
        return *this;
    }

    OpButtonRadioGroup & OpButtonRadioGroup::optionDirection( const OpButtonString & _value )
    {
        if( _value != "horizontal" && _value != "vertical" )
        {
            throw std::invalid_argument( "Invalid option direction" );
        }
        this->put_( "optionDirection", OpButtonJson( _value ) );
        return *this;
    }

    OpButtonRadioGroup::~OpButtonRadioGroup()
    {
    }
} // namespace OpButton
