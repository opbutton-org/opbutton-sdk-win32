#include "OpButton/OpButtonConnectResponse.h"

namespace OpButton {

OpButtonConnectResponse::OpButtonConnectResponse(const OpButtonString& requestId, const OpButtonString& status, const OpButtonString& sessionToken)
    : m_requestId(requestId), m_status(status), m_sessionToken(sessionToken) {
}

const OpButtonString& OpButtonConnectResponse::requestId() const { return m_requestId; }
const OpButtonString& OpButtonConnectResponse::status() const { return m_status; }
const OpButtonString& OpButtonConnectResponse::sessionToken() const { return m_sessionToken; }

bool OpButtonConnectResponse::isPending() const { return m_status == "pending"; }
bool OpButtonConnectResponse::isApproved() const { return m_status == "approved"; }
bool OpButtonConnectResponse::isRejected() const { return m_status == "rejected"; }

OpButtonConnectResponse OpButtonConnectResponse::fromJson(const OpButtonJson& json) {
    return OpButtonConnectResponse(
        json.optString("request_id"),
        json.optString("status"),
        json.optString("session_token")
    );
}

} // namespace OpButton
