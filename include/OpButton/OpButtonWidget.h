#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonLayoutChild.h"
#include "OpButton/OpButtonValueProvider.h"

namespace OpButton
{
    class OpButtonUI;

    class OpButtonWidget
        : public OpButtonLayoutChild
    {
    public:
        OpButtonWidget( OpButtonUI & _ui, const OpButtonString & _type );
        ~OpButtonWidget() override;

    public:
        int internalId() const;
        const OpButtonString & publicId() const;
        void setId( const OpButtonString & _id );
        const OpButtonString & widgetType() const;

        bool pollProvider();
        OpButtonJson toJson() const override;

    protected:
        void put_( const OpButtonString & _key, const OpButtonJson & _value );
        void setProvider_( const OpButtonString & _key, OpButtonValueProvider * _provider );

    protected:
        OpButtonUI & m_ui;

    private:
        int m_internalId;
        OpButtonString m_publicId;
        OpButtonString m_type;
        OpButtonJson m_json;

        OpButtonString m_providerKey;
        OpButtonValueProvider * m_provider = nullptr;
        OpButtonString m_cachedValue;
    };

} // namespace OpButton
