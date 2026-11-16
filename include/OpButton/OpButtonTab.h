#pragma once

#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonLayout.h"

namespace OpButton
{
    class OpButtonUI;

    class OpButtonTab
        : public OpButtonRefCounted
    {
    public:
        OpButtonTab( OpButtonUI & _ui, const OpButtonString & _title );
        ~OpButtonTab() override;

    public:
        OpButtonTab & setId( const OpButtonString & _id );

        OpButtonLayout & vertical();
        OpButtonLayout & horizontal();
        OpButtonLayout & grid();
        OpButtonLayout & setJson( const OpButtonJson & _json, const OpButtonJsonCallback & _callback = nullptr );

        const OpButtonString & title() const;
        const OpButtonString & id() const;
        int internalId() const;
        bool hasLayout() const;
        OpButtonLayout * getLayout() const;

        OpButtonJson toJson() const;

    private:
        OpButtonUI & m_ui;
        OpButtonString m_title;
        int m_internalId;
        OpButtonString m_id;
        OpButtonIntrusivePtr<OpButtonLayout> m_layout;
    };

} // namespace OpButton
