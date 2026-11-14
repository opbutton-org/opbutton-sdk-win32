#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonConnectResponse.h"
#include "OpButton/OpButtonLogEntry.h"
#include "OpButton/OpButtonMetricEntry.h"

namespace OpButton {

class OpButtonHttpClient {
public:
    OpButtonHttpClient(const OpButtonString& baseUrl, const OpButtonString& apiToken);
    ~OpButtonHttpClient();

    void shutdown();

    OpButtonConnectResponse connect(const OpButtonString& deviceId, const OpButtonString& deviceName,
                                 const OpButtonMap<OpButtonString, OpButtonJson>& sessionAttributes);
    OpButtonConnectResponse pollStatus(const OpButtonString& requestId);
    bool closeSession(const OpButtonString& sessionToken);
    OpButtonJson submitPanel(const OpButtonString& sessionToken, const OpButtonJson& layout);
    OpButtonJson pollActions(const OpButtonString& sessionToken,
                          const OpButtonMap<OpButtonString, OpButtonJson>* changedAttributes,
                          const OpButtonVector<OpButtonLogEntry>& logs,
                          const OpButtonVector<OpButtonMetricEntry>& metrics);
    void acknowledgeAction(const OpButtonString& sessionToken, const OpButtonString& actionId, const OpButtonJson& ackPayload);
    void sendLogs(const OpButtonString& sessionToken, const OpButtonVector<OpButtonLogEntry>& logs);
    void sendMetrics(const OpButtonString& sessionToken, const OpButtonVector<OpButtonMetricEntry>& metrics);
    OpButtonJson getLivePublisherState(const OpButtonString& sessionToken);

private:
    OpButtonJson executeRequest(const OpButtonString& method, const OpButtonString& path,
                             const OpButtonString& headerName, const OpButtonString& headerValue,
                             const OpButtonString& body);

    OpButtonString m_host;
    int m_port;
    OpButtonString m_basePath;
    bool m_secure;
    OpButtonString m_apiToken;

    void* m_hSession;  // HINTERNET
    void* m_hConnect;  // HINTERNET
};

} // namespace OpButton
