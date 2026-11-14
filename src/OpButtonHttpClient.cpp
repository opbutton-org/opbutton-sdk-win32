#include "OpButton/OpButtonHttpClient.h"
#include "OpButton/OpButtonException.h"
#include "OpButton/OpButtonLog.h"

#include "OpButtonWin32.h"
#include <wininet.h>

#include <cstring>
#include <cstdlib>

namespace OpButton {

// ── URL parsing helper ──

static void parseUrl(const OpButtonString& url, OpButtonString& host, int& port, OpButtonString& basePath, bool& secure) {
    OpButtonString remainder = url;
    secure = false;
    port = 80;

    if (remainder.substr(0, 8) == "https://") {
        secure = true;
        port = 443;
        remainder = remainder.substr(8);
    } else if (remainder.substr(0, 7) == "http://") {
        remainder = remainder.substr(7);
    }

    size_t slashPos = remainder.find('/');
    OpButtonString hostPort;
    if (slashPos != OpButtonString::npos) {
        hostPort = remainder.substr(0, slashPos);
        basePath = remainder.substr(slashPos);
    } else {
        hostPort = remainder;
        basePath = OpButtonString();
    }

    // Trim trailing slash from basePath
    while (!basePath.empty() && basePath.back() == '/') {
        basePath.pop_back();
    }

    size_t colonPos = hostPort.find(':');
    if (colonPos != OpButtonString::npos) {
        host = hostPort.substr(0, colonPos);
        port = atoi(hostPort.substr(colonPos + 1).c_str());
    } else {
        host = hostPort;
    }
}

// ── Constructor / Destructor ──

OpButtonHttpClient::OpButtonHttpClient(const OpButtonString& baseUrl, const OpButtonString& apiToken)
    : m_port(80), m_secure(false), m_apiToken(apiToken), m_hSession(nullptr), m_hConnect(nullptr) {
    parseUrl(baseUrl, m_host, m_port, m_basePath, m_secure);

    m_hSession = InternetOpenA("OpButtonSDK/1.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!m_hSession) {
        throw OpButtonException("Failed to initialize WinINet session");
    }

    m_hConnect = InternetConnectA(
        static_cast<HINTERNET>(m_hSession),
        m_host.c_str(),
        static_cast<INTERNET_PORT>(m_port),
        nullptr, nullptr,
        INTERNET_SERVICE_HTTP,
        0, 0);
    if (!m_hConnect) {
        InternetCloseHandle(static_cast<HINTERNET>(m_hSession));
        m_hSession = nullptr;
        throw OpButtonException("Failed to connect to " + m_host);
    }
}

OpButtonHttpClient::~OpButtonHttpClient() {
    shutdown();
}

void OpButtonHttpClient::shutdown() {
    if (m_hConnect) {
        InternetCloseHandle(static_cast<HINTERNET>(m_hConnect));
        m_hConnect = nullptr;
    }
    if (m_hSession) {
        InternetCloseHandle(static_cast<HINTERNET>(m_hSession));
        m_hSession = nullptr;
    }
}

// ── Core request execution ──

OpButtonJson OpButtonHttpClient::executeRequest(const OpButtonString& method, const OpButtonString& path,
                                          const OpButtonString& headerName, const OpButtonString& headerValue,
                                          const OpButtonString& body) {
    if (!m_hConnect) {
        throw OpButtonException("HTTP client is not connected");
    }

    OpButtonString fullPath = m_basePath + path;

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    if (m_secure) {
        flags |= INTERNET_FLAG_SECURE;
    }

    HINTERNET hRequest = HttpOpenRequestA(
        static_cast<HINTERNET>(m_hConnect),
        method.c_str(),
        fullPath.c_str(),
        nullptr, nullptr, nullptr,
        flags, 0);

    if (!hRequest) {
        throw OpButtonException("Failed to create HTTP request");
    }

    // Add content type header
    OpButtonString contentTypeHeader = "Content-Type: application/json; charset=utf-8\r\n";
    HttpAddRequestHeadersA(hRequest, contentTypeHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);

    // Add custom header (API token or session token)
    if (!headerName.empty()) {
        OpButtonString customHeader = headerName + OpButtonString(": ") + headerValue + OpButtonString("\r\n");
        HttpAddRequestHeadersA(hRequest, customHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);
    }

    // Send request
    BOOL result;
    if (body.empty()) {
        result = HttpSendRequestA(hRequest, nullptr, 0, nullptr, 0);
    } else {
        result = HttpSendRequestA(hRequest, nullptr, 0,
                                  const_cast<char*>(body.c_str()),
                                  static_cast<DWORD>(body.size()));
    }

    if (!result) {
        DWORD err = GetLastError();
        InternetCloseHandle(hRequest);
        throw OpButtonException("Network error: WinINet error code " + opbuttonToString(static_cast<unsigned long>(err)));
    }

    // Read status code
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                   &statusCode, &statusCodeSize, nullptr);

    // Read response body
    OpButtonString responseBody;
    char readBuf[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hRequest, readBuf, sizeof(readBuf), &bytesRead) && bytesRead > 0) {
        responseBody.append(readBuf, bytesRead);
    }

    InternetCloseHandle(hRequest);

    if (statusCode < 200 || statusCode >= 300) {
        OpButtonString detail = responseBody;
        OpButtonJson errorJson = OpButtonJson::parse(responseBody);
        OpButtonString detailStr = errorJson.optString("detail");
        if (!detailStr.empty()) {
            detail = detailStr;
        }
        throw OpButtonException(static_cast<int>(statusCode),
                             OpButtonString("HTTP ") + opbuttonToString(statusCode) + OpButtonString(": ") + detail);
    }

    if (responseBody.empty()) return OpButtonJson::object();
    return OpButtonJson::parse(responseBody);
}

// ── Client endpoints ──

OpButtonConnectResponse OpButtonHttpClient::connect(const OpButtonString& deviceId, const OpButtonString& deviceName,
                                               const OpButtonMap<OpButtonString, OpButtonJson>& sessionAttributes) {
    OpButtonJson body = OpButtonJson::object()
        .put("device_id", OpButtonJson(deviceId))
        .put("device_name", OpButtonJson(deviceName));

    if (!sessionAttributes.empty()) {
        OpButtonJson attrs = OpButtonJson::object();
        for (const auto& kv : sessionAttributes) {
            attrs.put(kv.first, kv.second);
        }
        body.put("session_attributes", attrs);
    }

    OpButtonJson resp = executeRequest("POST", "/api/client/connect", "X-Api-Token", m_apiToken, body.toString());
    return OpButtonConnectResponse::fromJson(resp);
}

OpButtonConnectResponse OpButtonHttpClient::pollStatus(const OpButtonString& requestId) {
    OpButtonJson resp = executeRequest("GET", "/api/client/poll-status/" + requestId,
                                    "X-Api-Token", m_apiToken, "");
    return OpButtonConnectResponse::fromJson(resp);
}

bool OpButtonHttpClient::closeSession(const OpButtonString& sessionToken) {
    OpButtonJson resp = executeRequest("POST", "/api/client/session/close",
                                    "X-Session-Token", sessionToken, "");
    return resp.optBool("ok", false);
}

OpButtonJson OpButtonHttpClient::submitPanel(const OpButtonString& sessionToken, const OpButtonJson& layout) {
    OpButtonJson body = OpButtonJson::object().put("layout", layout);
    return executeRequest("POST", "/api/client/session/panel",
                          "X-Session-Token", sessionToken, body.toString());
}

OpButtonJson OpButtonHttpClient::pollActions(const OpButtonString& sessionToken,
                                        const OpButtonMap<OpButtonString, OpButtonJson>* changedAttributes,
                                        const OpButtonVector<OpButtonLogEntry>& logs,
                                        const OpButtonVector<OpButtonMetricEntry>& metrics) {
    OpButtonJson body = OpButtonJson::object();

    if (changedAttributes && !changedAttributes->empty()) {
        OpButtonJson attrs = OpButtonJson::object();
        for (const auto& kv : *changedAttributes) {
            attrs.put(kv.first, kv.second);
        }
        body.put("session_attributes", attrs);
    }

    if (!logs.empty()) {
        OpButtonJson logsArr = OpButtonJson::array();
        for (const auto& entry : logs) {
            logsArr.add(entry.toJson());
        }
        body.put("logs", logsArr);
    }

    if (!metrics.empty()) {
        OpButtonJson metricsArr = OpButtonJson::array();
        for (const auto& entry : metrics) {
            metricsArr.add(entry.toJson());
        }
        body.put("metrics", metricsArr);
    }

    OpButtonString bodyStr = (body.objectSize() > 0) ? body.toString() : OpButtonString();
    return executeRequest("POST", "/api/client/session/actions/poll",
                          "X-Session-Token", sessionToken, bodyStr);
}

void OpButtonHttpClient::acknowledgeAction(const OpButtonString& sessionToken,
                                         const OpButtonString& actionId,
                                         const OpButtonJson& ackPayload) {
    OpButtonJson body = OpButtonJson::object()
        .put("action_id", OpButtonJson(actionId))
        .put("ack_payload", ackPayload.isNull() ? OpButtonJson::object() : ackPayload);

    executeRequest("POST", "/api/client/session/actions/ack",
                   "X-Session-Token", sessionToken, body.toString());
}

void OpButtonHttpClient::sendLogs(const OpButtonString& sessionToken, const OpButtonVector<OpButtonLogEntry>& logs) {
    if (logs.empty()) return;

    OpButtonJson logsArr = OpButtonJson::array();
    for (const auto& entry : logs) {
        logsArr.add(entry.toJson());
    }

    OpButtonJson body = OpButtonJson::object().put("logs", logsArr);
    executeRequest("POST", "/api/client/session/logs",
                   "X-Session-Token", sessionToken, body.toString());
}

void OpButtonHttpClient::sendMetrics(const OpButtonString& sessionToken, const OpButtonVector<OpButtonMetricEntry>& metrics) {
    if (metrics.empty()) return;

    OpButtonJson metricsArr = OpButtonJson::array();
    for (const auto& entry : metrics) {
        metricsArr.add(entry.toJson());
    }

    OpButtonJson body = OpButtonJson::object().put("metrics", metricsArr);
    executeRequest("POST", "/api/client/session/metrics",
                   "X-Session-Token", sessionToken, body.toString());
}

OpButtonJson OpButtonHttpClient::getLivePublisherState(const OpButtonString& sessionToken) {
    return executeRequest("GET", "/api/client/session/live/publisher",
                          "X-Session-Token", sessionToken, "");
}

} // namespace OpButton
