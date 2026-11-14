#include "OpButton/OpButtonAction.h"

namespace OpButton {

OpButtonAction::OpButtonAction()
    : m_widgetId(0), m_actionType(OpButtonActionType::UNKNOWN), m_status(OpButtonActionStatus::UNKNOWN) {
}

OpButtonAction::OpButtonAction(const OpButtonString& id, const OpButtonString& sessionId, int widgetId,
                         OpButtonActionType actionType, OpButtonActionStatus status, const OpButtonJson& payload)
    : m_id(id), m_sessionId(sessionId), m_widgetId(widgetId)
    , m_actionType(actionType), m_status(status), m_payload(payload) {
}

const OpButtonString& OpButtonAction::id() const { return m_id; }
const OpButtonString& OpButtonAction::sessionId() const { return m_sessionId; }
int OpButtonAction::widgetId() const { return m_widgetId; }
OpButtonActionType OpButtonAction::actionType() const { return m_actionType; }
OpButtonActionStatus OpButtonAction::actionStatus() const { return m_status; }
const OpButtonJson& OpButtonAction::payload() const { return m_payload; }

OpButtonAction OpButtonAction::fromJson(const OpButtonJson& json) {
    return OpButtonAction(
        json.optString("id"),
        json.optString("session_id"),
        json.optInt("widget_id", 0),
        opbuttonActionTypeFromValue(json.optString("action_type")),
        opbuttonActionStatusFromValue(json.optString("status")),
        json.optObject("payload")
    );
}

} // namespace OpButton
