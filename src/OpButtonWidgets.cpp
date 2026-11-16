#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonInput.h"
#include "OpButton/OpButtonLabel.h"
#include "OpButton/OpButtonLogs.h"
#include "OpButton/OpButtonSelect.h"
#include "OpButton/OpButtonStat.h"
#include "OpButton/OpButtonSwitch.h"
#include "OpButton/OpButtonTable.h"
#include "OpButton/OpButtonTextarea.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton::OpButtonButton( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "button" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton & OpButtonButton::variant( const OpButtonString & _v )
    {
        this->put_( "variant", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton & OpButtonButton::color( const OpButtonString & _c )
    {
        this->put_( "color", OpButtonJson( _c ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton & OpButtonButton::disabled( bool _d )
    {
        this->put_( "disabled", OpButtonJson( _d ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton & OpButtonButton::onClick( OpButtonWidgetCallback * _callback )
    {
        m_ui.registerCallback( this->internalId(), _callback );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton & OpButtonButton::onClick( OpButtonWidgetCallbackFunc _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), std::move( _callback ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitch::OpButtonSwitch( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "switch" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitch & OpButtonSwitch::defaultValue( bool _v )
    {
        this->put_( "defaultValue", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitch & OpButtonSwitch::onChange( OpButtonWidgetCallback * _callback )
    {
        m_ui.registerCallback( this->internalId(), _callback );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitch & OpButtonSwitch::onChange( OpButtonWidgetCallbackFunc _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), std::move( _callback ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitch & OpButtonSwitch::onChange( OpButtonFunction<void( const OpButtonSwitchAction & )> _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), [cb = std::move( _callback )]( const OpButtonAction & _action )
                                   { cb( OpButtonSwitchAction( _action ) ); } );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput::OpButtonInput( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "input" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput & OpButtonInput::inputType( const OpButtonString & _t )
    {
        this->put_( "inputType", OpButtonJson( _t ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput & OpButtonInput::defaultValue( const OpButtonString & _v )
    {
        this->put_( "defaultValue", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput & OpButtonInput::placeholder( const OpButtonString & _p )
    {
        this->put_( "placeholder", OpButtonJson( _p ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput & OpButtonInput::onSubmit( OpButtonWidgetCallback * _callback )
    {
        m_ui.registerCallback( this->internalId(), _callback );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput & OpButtonInput::onSubmit( OpButtonWidgetCallbackFunc _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), std::move( _callback ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput & OpButtonInput::onSubmit( OpButtonFunction<void( const OpButtonInputAction & )> _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), [cb = std::move( _callback )]( const OpButtonAction & _action )
                                   { cb( OpButtonInputAction( _action ) ); } );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelect::OpButtonSelect( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "select" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelect & OpButtonSelect::options( const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>> & _opts )
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
    OpButtonSelect & OpButtonSelect::defaultValue( const OpButtonString & _v )
    {
        this->put_( "defaultValue", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelect & OpButtonSelect::onChange( OpButtonWidgetCallback * _callback )
    {
        m_ui.registerCallback( this->internalId(), _callback );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelect & OpButtonSelect::onChange( OpButtonWidgetCallbackFunc _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), std::move( _callback ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelect & OpButtonSelect::onChange( OpButtonFunction<void( const OpButtonSelectAction & )> _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), [cb = std::move( _callback )]( const OpButtonAction & _action )
                                   { cb( OpButtonSelectAction( _action ) ); } );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea::OpButtonTextarea( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "textarea" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea & OpButtonTextarea::rows( int _r )
    {
        this->put_( "rows", OpButtonJson( _r ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea & OpButtonTextarea::defaultValue( const OpButtonString & _v )
    {
        this->put_( "defaultValue", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea & OpButtonTextarea::onSubmit( OpButtonWidgetCallback * _callback )
    {
        m_ui.registerCallback( this->internalId(), _callback );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea & OpButtonTextarea::onSubmit( OpButtonWidgetCallbackFunc _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), std::move( _callback ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea & OpButtonTextarea::onSubmit( OpButtonFunction<void( const OpButtonTextareaAction & )> _callback )
    {
        m_ui.registerCallbackFunc( this->internalId(), [cb = std::move( _callback )]( const OpButtonAction & _action )
                                   { cb( OpButtonTextareaAction( _action ) ); } );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLabel::OpButtonLabel( OpButtonUI & _ui, const OpButtonString & _text )
        : OpButtonWidget( _ui, "label" )
    {
        this->put_( "text", OpButtonJson( _text ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLabel & OpButtonLabel::text( const OpButtonString & _t )
    {
        this->put_( "text", OpButtonJson( _t ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLabel & OpButtonLabel::color( const OpButtonString & _c )
    {
        this->put_( "color", OpButtonJson( _c ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLabel & OpButtonLabel::textProvider( OpButtonValueProvider * _provider )
    {
        this->setProvider_( "text", _provider );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonStat::OpButtonStat( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "stat" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonStat & OpButtonStat::value( const OpButtonString & _v )
    {
        this->put_( "value", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonStat & OpButtonStat::unit( const OpButtonString & _u )
    {
        this->put_( "unit", OpButtonJson( _u ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonStat & OpButtonStat::valueProvider( OpButtonValueProvider * _provider )
    {
        this->setProvider_( "value", _provider );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTable::OpButtonTable( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "table" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTable & OpButtonTable::columns( const OpButtonVector<OpButtonPair<OpButtonString, OpButtonString>> & _cols )
    {
        OpButtonJson arr = OpButtonJson::array();
        for( const auto & col : _cols )
        {
            arr.add( OpButtonJson::object().put( "key", OpButtonJson( col.first ) ).put( "label", OpButtonJson( col.second ) ) );
        }
        this->put_( "columns", arr );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTable & OpButtonTable::rows( const OpButtonVector<OpButtonJson> & _rowData )
    {
        OpButtonJson arr = OpButtonJson::array();
        for( const auto & row : _rowData )
        {
            arr.add( row );
        }
        this->put_( "rows", arr );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogs::OpButtonLogs( OpButtonUI & _ui, const OpButtonString & _label )
        : OpButtonWidget( _ui, "logs" )
    {
        this->put_( "label", OpButtonJson( _label ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogs & OpButtonLogs::maxLines( int _v )
    {
        this->put_( "maxLines", OpButtonJson( _v ) );
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton::~OpButtonButton()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput::~OpButtonInput()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLabel::~OpButtonLabel()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogs::~OpButtonLogs()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelect::~OpButtonSelect()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonStat::~OpButtonStat()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitch::~OpButtonSwitch()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTable::~OpButtonTable()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea::~OpButtonTextarea()
    {
    }
} // namespace OpButton
