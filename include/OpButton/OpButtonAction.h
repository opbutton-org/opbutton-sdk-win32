#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonActionType.h"
#include "OpButton/OpButtonActionStatus.h"

namespace OpButton {

class OpButtonAction {
public:
    OpButtonAction();
    OpButtonAction(const OpButtonString& id, const OpButtonString& sessionId, int widgetId,
                OpButtonActionType actionType, OpButtonActionStatus status, const OpButtonJson& payload);

    const OpButtonString& id() const;
    const OpButtonString& sessionId() const;
    int widgetId() const;
    OpButtonActionType actionType() const;
    OpButtonActionStatus actionStatus() const;
    const OpButtonJson& payload() const;

    static OpButtonAction fromJson(const OpButtonJson& json);

private:
    OpButtonString m_id;
    OpButtonString m_sessionId;
    int m_widgetId;
    OpButtonActionType m_actionType;
    OpButtonActionStatus m_status;
    OpButtonJson m_payload;
};

} // namespace OpButton
