#include "OpButton/OpButtonSlider.h"
#include "OpButton/OpButtonUI.h"
#include <cmath>
#include <stdexcept>

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonSlider::OpButtonSlider( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "slider" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    OpButtonSlider & OpButtonSlider::range( double _min, double _max, double _step )
    {
        if( std::isfinite( _min ) == false || std::isfinite( _max ) == false || std::isfinite( _step ) == false || std::isfinite( _max - _min ) == false || _min >= _max || _step <= 0 || _step > _max - _min )
        {
            throw std::invalid_argument( "Invalid slider range" );
        }
        this->put_( "min", OpButtonJson( _min ) );
        this->put_( "max", OpButtonJson( _max ) );
        this->put_( "step", OpButtonJson( _step ) );
        return *this;
    }

    OpButtonSlider & OpButtonSlider::defaultValue( double _value )
    {
        if( std::isfinite( _value ) == false )
        {
            throw std::invalid_argument( "Invalid slider value" );
        }
        this->put_( "defaultValue", OpButtonJson( _value ) );
        return *this;
    }

    OpButtonSlider & OpButtonSlider::disabled( bool _value )
    {
        this->put_( "disabled", OpButtonJson( _value ) );
        return *this;
    }

    OpButtonSlider & OpButtonSlider::unit( const OpButtonString & _value )
    {
        this->put_( "unit", OpButtonJson( _value ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSlider & OpButtonSlider::onChange( OpButtonWidgetCallback * _callback )
    {
        m_ui.registerCallback( this->internalId(), _callback );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSlider & OpButtonSlider::onChange( OpButtonWidgetCallbackFunc _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), std::move( _callback ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSlider & OpButtonSlider::onChange( OpButtonFunction<void( const OpButtonSliderAction & )> _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), [cb = std::move( _callback )]( const OpButtonAction & _action )
                                   { cb( OpButtonSliderAction( _action ) ); } );
        return *this;
    }

    OpButtonSlider::~OpButtonSlider()
    {
    }
} // namespace OpButton
