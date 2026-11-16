#include "OpButton/OpButtonAction.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonAction::OpButtonAction()
        : m_widgetId( 0 )
        , m_actionType( OpButtonActionType::UNKNOWN )
        , m_status( OpButtonActionStatus::UNKNOWN )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonAction::OpButtonAction( const OpButtonString & _id, const OpButtonString & _sessionId, int _widgetId, OpButtonActionType _actionType, OpButtonActionStatus _status, const OpButtonJson & _payload )
        : m_id( _id )
        , m_sessionId( _sessionId )
        , m_widgetId( _widgetId )
        , m_actionType( _actionType )
        , m_status( _status )
        , m_payload( _payload )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonAction::id() const
    {
        return m_id;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonAction::sessionId() const
    {
        return m_sessionId;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonAction::widgetId() const
    {
        return m_widgetId;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonActionType OpButtonAction::actionType() const
    {
        return m_actionType;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonActionStatus OpButtonAction::actionStatus() const
    {
        return m_status;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonJson & OpButtonAction::payload() const
    {
        return m_payload;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonAction OpButtonAction::fromJson( const OpButtonJson & _json )
    {
        return OpButtonAction( _json.optString( "id" ), _json.optString( "session_id" ), _json.optInt( "widget_id", 0 ), opbuttonActionTypeFromValue( _json.optString( "action_type" ) ),
                               opbuttonActionStatusFromValue( _json.optString( "status" ) ), _json.optObject( "payload" ) );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonAction::~OpButtonAction()
    {
    }
} // namespace OpButton
