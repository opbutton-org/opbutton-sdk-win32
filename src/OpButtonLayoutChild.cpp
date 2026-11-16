#include "OpButton/OpButtonLayoutChild.h"
#include "OpButton/OpButtonUI.h"
#include <cmath>
#include <stdexcept>

namespace OpButton
{
    OpButtonLayoutChild::OpButtonLayoutChild( OpButtonUI & _ui )
        : m_ui( _ui )
        , m_elementId( _ui.nextId() )
        , m_placement( OpButtonJson::object() )
    {
    }

    OpButtonLayoutChild::~OpButtonLayoutChild()
    {
    }

    int OpButtonLayoutChild::internalId() const
    {
        return m_elementId;
    }

    double OpButtonLayoutChild::dimension_( double _value, bool _positive )
    {
        if( std::isfinite( _value ) == false || _value > 4096 || ( _positive ? _value <= 0 : _value < 0 ) )
        {
            throw std::invalid_argument( "Invalid layout dimension" );
        }
        return _value;
    }

    const OpButtonString & OpButtonLayoutChild::alignment_( const OpButtonString & _value )
    {
        if( _value != "start" && _value != "center" && _value != "end" && _value != "stretch" )
        {
            throw std::invalid_argument( "Invalid layout alignment" );
        }
        return _value;
    }

    OpButtonLayoutChild & OpButtonLayoutChild::place_( const OpButtonString & _key, const OpButtonJson & _value )
    {
        m_placement.put( _key, _value );
        m_ui.incrementRevision();
        return *this;
    }

    OpButtonLayoutChild & OpButtonLayoutChild::width( double _pixels )
    {
        return this->place_( "width", OpButtonJson( dimension_( _pixels, true ) ) );
    }

    OpButtonLayoutChild & OpButtonLayoutChild::contentWidth()
    {
        return this->place_( "width", OpButtonJson( "content" ) );
    }

    OpButtonLayoutChild & OpButtonLayoutChild::fill( double _weight )
    {
        this->place_( "weight", OpButtonJson( dimension_( _weight, true ) ) );
        return this->place_( "width", OpButtonJson( "fill" ) );
    }

    OpButtonLayoutChild & OpButtonLayoutChild::minWidth( double _pixels )
    {
        return this->place_( "minWidth", OpButtonJson( dimension_( _pixels ) ) );
    }

    OpButtonLayoutChild & OpButtonLayoutChild::maxWidth( double _pixels )
    {
        return this->place_( "maxWidth", OpButtonJson( dimension_( _pixels, true ) ) );
    }

    OpButtonLayoutChild & OpButtonLayoutChild::alignSelf( const OpButtonString & _value )
    {
        return this->place_( "alignSelf", OpButtonJson( alignment_( _value ) ) );
    }

    void OpButtonLayoutChild::loadPlacement_( const OpButtonJson & _json )
    {
        m_placement = _json.optObject( "placement" );
    }

    OpButtonJson OpButtonLayoutChild::decorate_( OpButtonJson _json ) const
    {
        _json.put( "id", OpButtonJson( m_elementId ) );
        if( m_placement.objectSize() != 0 )
        {
            _json.put( "placement", m_placement );
        }
        return _json;
    }
} // namespace OpButton
