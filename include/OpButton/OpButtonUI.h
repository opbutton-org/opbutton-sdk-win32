#pragma once

#include "OpButton/OpButtonAction.h"
#include "OpButton/OpButtonCallbackFunction.h"
#include "OpButton/OpButtonJson.h"
#include "OpButton/OpButtonWidgetCallback.h"

namespace OpButton
{
    class OpButtonTab;
    class OpButtonWidget;

    class OpButtonUI : public OpButtonRefCounted
    {
    public:
        OpButtonUI();
        ~OpButtonUI() override;
        int nextId();

        // Tab management
        OpButtonTab & addTab( const OpButtonString & _title );
        OpButtonTab * getTab( const OpButtonString & _id );
        void removeTab( const OpButtonString & _id );

        // Widget callbacks
        // Raw callbacks are not owned and must remain alive during dispatch.
        void registerCallback( int _widgetId, OpButtonWidgetCallback * _callback );
        void registerCallbackFunc( int _widgetId, OpButtonWidgetCallbackFunc _func );
        void removeCallback( int _widgetId );
        bool dispatchAction( const OpButtonAction & _action );

        // Value providers
        void registerProvider( OpButtonWidget * _widget );
        void unregisterProvider( OpButtonWidget * _widget );
        void pollValues();

        // Revision tracking
        void incrementRevision();
        bool hasUnsent() const;
        void markSent();
        bool hasTabs() const;
        int getRevision() const;

        // Serialization
        OpButtonJson toJson();

    private:
        OpButtonVector<OpButtonIntrusivePtr<OpButtonTab>> m_tabs;
        OpButtonMutex m_tabMutex;

        struct CallbackEntry
        {
        public:
            CallbackEntry();
            ~CallbackEntry();

            OpButtonWidgetCallback * raw = nullptr;
            OpButtonIntrusivePtr<OpButtonCallbackFunction> func;
        };

        void setCallbackEntry_( int _widgetId, CallbackEntry _entry );

        OpButtonUnorderedMap<int, CallbackEntry> m_callbacks;
        OpButtonMutex m_callbackMutex;

        OpButtonVector<OpButtonWidget *> m_providers;
        OpButtonMutex m_providerMutex;

        OpButtonAtomic<int> m_idCounter{ 0 };
        OpButtonAtomic<int> m_revision{ 1 };
        int m_sentRevision = 0;
        static const int VERSION = 2;
    };

} // namespace OpButton
