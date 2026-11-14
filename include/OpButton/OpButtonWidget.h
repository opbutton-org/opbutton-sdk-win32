#pragma once

#include "OpButton/OpButtonLayoutChild.h"
#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonValueProvider.h"

namespace OpButton {

class OpButtonUI;

class OpButtonWidget : public OpButtonLayoutChild {
public:
    OpButtonWidget(OpButtonUI& ui, const OpButtonString& type);
    ~OpButtonWidget() override = default;

    int internalId() const;
    const OpButtonString& publicId() const;
    void setId(const OpButtonString& id);
    const OpButtonString& widgetType() const;

    bool pollProvider();
    OpButtonJson toJson() const override;

protected:
    void put(const OpButtonString& key, const OpButtonJson& value);
    void setProvider(const OpButtonString& key, OpButtonValueProvider* provider);

    OpButtonUI& m_ui;

private:
    int m_internalId;
    OpButtonString m_publicId;
    OpButtonString m_type;
    OpButtonJson m_json;

    OpButtonString m_providerKey;
    OpButtonValueProvider* m_provider = nullptr;
    OpButtonString m_cachedValue;
};

} // namespace OpButton
