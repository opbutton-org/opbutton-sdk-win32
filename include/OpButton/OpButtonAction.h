#pragma once

#include "OpButton/OpButtonActionStatus.h"
#include "OpButton/OpButtonActionType.h"
#include "OpButton/OpButtonJson.h"

namespace OpButton
{
    class OpButtonAction
    {
    public:
        OpButtonAction();
        OpButtonAction( const OpButtonString & _id, const OpButtonString & _sessionId, int _widgetId, OpButtonActionType _actionType, OpButtonActionStatus _status, const OpButtonJson & _payload );
        ~OpButtonAction();

    public:
        const OpButtonString & id() const;
        const OpButtonString & sessionId() const;
        int widgetId() const;
        OpButtonActionType actionType() const;
        OpButtonActionStatus actionStatus() const;
        const OpButtonJson & payload() const;

        static OpButtonAction fromJson( const OpButtonJson & _json );

    private:
        OpButtonString m_id;
        OpButtonString m_sessionId;
        int m_widgetId;
        OpButtonActionType m_actionType;
        OpButtonActionStatus m_status;
        OpButtonJson m_payload;
    };

} // namespace OpButton
