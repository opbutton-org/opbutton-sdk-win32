#pragma once

#include "OpButton/OpButtonActionListener.h"
#include "OpButton/OpButtonLogConfigBuilder.h"
#include "OpButton/OpButtonLoggerListener.h"
#include "OpButton/OpButtonMetricConfigBuilder.h"
#include "OpButton/OpButtonSessionAttributeBuilder.h"
#include "OpButton/OpButtonSessionListener.h"

namespace OpButton
{
    class OpButtonConfig
    {
    public:
        ~OpButtonConfig();

    public:
        class Builder;
        friend class Builder;

    public:
        const OpButtonString & baseUrl() const;
        const OpButtonString & apiToken() const;
        const OpButtonString & deviceId() const;
        const OpButtonString & deviceName() const;
        int64_t pollIntervalMs() const;
        int64_t actionPollIntervalMs() const;
        bool autoConnect() const;
        OpButtonLoggerListener * loggerListener() const;
        OpButtonSessionListener * sessionListener() const;
        OpButtonActionListener * actionListener() const;
        const OpButtonSessionAttributeBuilder & sessionAttributes() const;
        const OpButtonLogConfigBuilder & logConfig() const;
        const OpButtonMetricConfigBuilder & metricConfig() const;

    private:
        OpButtonConfig();

    private:
        OpButtonString m_baseUrl;
        OpButtonString m_apiToken;
        OpButtonString m_deviceId;
        OpButtonString m_deviceName;
        int64_t m_pollIntervalMs = 10000;
        int64_t m_actionPollIntervalMs = 2000;
        bool m_autoConnect = true;
        OpButtonLoggerListener * m_loggerListener = nullptr;
        OpButtonSessionListener * m_sessionListener = nullptr;
        OpButtonActionListener * m_actionListener = nullptr;
        OpButtonSessionAttributeBuilder m_sessionAttributes;
        OpButtonLogConfigBuilder m_logConfig;
        OpButtonMetricConfigBuilder m_metricConfig;
    };

} // namespace OpButton

#include "OpButton/OpButtonConfigBuilder.h"
