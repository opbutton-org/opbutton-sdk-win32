#include "OpButton/OpButtonWidget.h"
#include "OpButton/OpButtonLog.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonWidget::OpButtonWidget( OpButtonUI & _ui, const OpButtonString & _type )
        : m_ui( _ui )
        , m_internalId( _ui.nextId() )
        , m_publicId( _type + OpButtonString( "-" ) + opbuttonToString( m_internalId ) )
        , m_type( _type )
        , m_json( OpButtonJson::object() )
    {
        m_json.put( "type", OpButtonJson( _type ) );
        m_json.put( "id", OpButtonJson( m_internalId ) );
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonWidget::internalId() const
    {
        return m_internalId;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonWidget::publicId() const
    {
        return m_publicId;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonWidget::setId( const OpButtonString & _id )
    {
        m_publicId = _id;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonWidget::widgetType() const
    {
        return m_type;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonWidget::toJson() const
    {
        return m_json;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonWidget::put_( const OpButtonString & _key, const OpButtonJson & _value )
    {
        m_json.put( _key, _value );
        m_ui.incrementRevision();
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonWidget::setProvider_( const OpButtonString & _key, OpButtonValueProvider * _provider )
    {
        m_providerKey = _key;
        m_provider = _provider;
        if( _provider != nullptr )
        {
            m_ui.registerProvider( this );
        }
        else
        {
            m_ui.unregisterProvider( this );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonWidget::pollProvider()
    {
        if( m_provider == nullptr )
        {
            return false;
        }
        try
        {
            OpButtonString newValue = m_provider->getValue();
            if( newValue != m_cachedValue )
            {
                m_cachedValue = newValue;
                if( m_providerKey.empty() == false )
                {
                    m_json.put( m_providerKey, OpButtonJson( newValue ) );
                }
                return true;
            }
        }
        catch( ... )
        {
            OpButtonLog::e( "Value provider failed for widget %d", m_internalId );
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonWidget::~OpButtonWidget()
    {
        m_ui.unregisterProvider( this );
        m_ui.removeCallback( m_internalId );
    }
} // namespace OpButton
