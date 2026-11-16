#include "OpButtonJsonElement.h"

#include "OpButton/OpButtonUI.h"

#include <stdexcept>

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    OpButtonJsonElement::OpButtonJsonElement( OpButtonUI & _ui, const OpButtonJson & _json, int _depth )
        : OpButtonLayoutChild( _ui )
        , m_json( OpButtonJson::object() )
    {
        if( _depth > 32 )
        {
            throw std::invalid_argument( "OpButton JSON nesting exceeds 32" );
        }

        if( _json.type() != OpButtonJson::TYPE_OBJECT )
        {
            throw std::invalid_argument( "OpButton JSON element must be an object" );
        }

        if( _json.optString( "type" ).empty() == true )
        {
            throw std::invalid_argument( "OpButton JSON element requires a type" );
        }

        for( const auto & entry : _json.objectEntries() )
        {
            if( entry.first == "children" )
            {
                continue;
            }

            m_json.put( entry.first, entry.second );
        }

        OpButtonJson identifier = _json.get( "id" );

        if( identifier.type() == OpButtonJson::TYPE_STRING )
        {
            m_identifier = identifier.asString();
        }

        if( _json.has( "children" ) == true )
        {
            OpButtonJson children = _json.get( "children" );

            if( children.type() != OpButtonJson::TYPE_ARRAY )
            {
                throw std::invalid_argument( "OpButton JSON children must be an array" );
            }

            m_json.put( "children", OpButtonJson::array() );

            for( int index = 0; index != children.length(); ++index )
            {
                m_children.emplace_back( new OpButtonJsonElement( _ui, children.at( index ), _depth + 1 ) );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OpButtonJsonElement::~OpButtonJsonElement()
    {
        m_ui.removeCallback( this->internalId() );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpButtonJsonElement::attach( const OpButtonJsonCallback & _callback )
    {
        if( m_identifier.empty() == false && _callback != nullptr )
        {
            m_ui.registerCallbackFunc( this->internalId(), [identifier = m_identifier, callback = _callback]( const OpButtonAction & _action ) {
                callback( identifier, _action );
            } );
        }

        for( const OpButtonIntrusivePtr<OpButtonJsonElement> & child : m_children )
        {
            child->attach( _callback );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonJsonElement::toJson() const
    {
        OpButtonJson json = m_json;

        if( json.has( "children" ) == true )
        {
            OpButtonJson children = OpButtonJson::array();

            for( const OpButtonIntrusivePtr<OpButtonJsonElement> & child : m_children )
            {
                children.add( child->toJson() );
            }

            json.put( "children", children );
        }

        OpButtonJson result = this->decorate_( json );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
}
