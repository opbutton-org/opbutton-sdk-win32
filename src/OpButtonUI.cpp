#include "OpButton/OpButtonUI.h"
#include "OpButton/OpButtonLog.h"
#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonWidget.h"

#include <algorithm>

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonUI::OpButtonUI()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonUI::~OpButtonUI()
    {
        // Widgets unregister while all registries and their mutexes still exist.
        OpButtonVector<OpButtonIntrusivePtr<OpButtonTab>> tabs;
        tabs.swap( m_tabs );
        tabs.clear();
        // Release callback captures before the registry itself starts destruction.
        OpButtonUnorderedMap<int, CallbackEntry> callbacks;
        callbacks.swap( m_callbacks );
        callbacks.clear();
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonUI::nextId()
    {
        return ++m_idCounter;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTab & OpButtonUI::addTab( const OpButtonString & _title )
    {
        OpButtonIntrusivePtr<OpButtonTab> tab( new OpButtonTab( *this, _title ) );
        OpButtonIntrusivePtr<OpButtonTab> previous;
        {
            std::lock_guard<OpButtonMutex> lock( m_tabMutex );
            for( OpButtonIntrusivePtr<OpButtonTab> & current : m_tabs )
            {
                if( current->title() != _title )
                {
                    continue;
                }
                previous.swap( current );
                current = tab;
                break;
            }
            if( previous == nullptr )
            {
                m_tabs.push_back( tab );
            }
            ++m_revision;
        }
        return *tab;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTab * OpButtonUI::getTab( const OpButtonString & _id )
    {
        std::lock_guard<OpButtonMutex> lock( m_tabMutex );
        for( auto & t : m_tabs )
        {
            if( t->id() == _id )
            {
                return t.get();
            }
        }
        return nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::removeTab( const OpButtonString & _id )
    {
        OpButtonIntrusivePtr<OpButtonTab> removed;
        {
            std::lock_guard<OpButtonMutex> lock( m_tabMutex );
            for( OpButtonVector<OpButtonIntrusivePtr<OpButtonTab>>::iterator it = m_tabs.begin(); it != m_tabs.end(); ++it )
            {
                if( ( *it )->id() != _id )
                {
                    continue;
                }
                removed.swap( *it );
                m_tabs.erase( it );
                break;
            }
            ++m_revision;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::registerCallback( int _widgetId, OpButtonWidgetCallback * _callback )
    {
        CallbackEntry entry;
        entry.raw = _callback;
        this->setCallbackEntry_( _widgetId, std::move( entry ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::registerCallbackFunc( int _widgetId, OpButtonWidgetCallbackFunc _func )
    {
        CallbackEntry entry;
        if( _func != nullptr )
        {
            entry.func = OpButtonIntrusivePtr<OpButtonCallbackFunction>( new OpButtonCallbackFunction( std::move( _func ) ) );
        }
        this->setCallbackEntry_( _widgetId, std::move( entry ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::removeCallback( int _widgetId )
    {
        CallbackEntry entry;
        this->setCallbackEntry_( _widgetId, std::move( entry ) );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::setCallbackEntry_( int _widgetId, CallbackEntry _entry )
    {
        bool hasCallback = _entry.raw != nullptr || static_cast<bool>( _entry.func );
        std::lock_guard<OpButtonMutex> lock( m_callbackMutex );
        OpButtonUnorderedMap<int, CallbackEntry>::iterator it = m_callbacks.find( _widgetId );
        if( it != m_callbacks.end() )
        {
            // The old handler moves into the parameter, which outlives the lock.
            std::swap( _entry, it->second );
            if( hasCallback == false )
            {
                m_callbacks.erase( it );
            }
        }
        else if( hasCallback == true )
        {
            m_callbacks.emplace( _widgetId, std::move( _entry ) );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonUI::dispatchAction( const OpButtonAction & _action )
    {
        CallbackEntry entry;
        {
            std::lock_guard<OpButtonMutex> lock( m_callbackMutex );
            OpButtonUnorderedMap<int, CallbackEntry>::iterator it = m_callbacks.find( _action.widgetId() );
            if( it == m_callbacks.end() )
            {
                return false;
            }

            entry = it->second;
        }

        if( entry.func != nullptr )
        {
            entry.func->invoke( _action );
        }
        else if( entry.raw != nullptr )
        {
            entry.raw->onOpButtonWidgetAction( _action );
        }
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::registerProvider( OpButtonWidget * _widget )
    {
        std::lock_guard<OpButtonMutex> lock( m_providerMutex );
        if( std::find( m_providers.begin(), m_providers.end(), _widget ) == m_providers.end() )
        {
            m_providers.push_back( _widget );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::unregisterProvider( OpButtonWidget * _widget )
    {
        std::lock_guard<OpButtonMutex> lock( m_providerMutex );
        m_providers.erase( std::remove( m_providers.begin(), m_providers.end(), _widget ), m_providers.end() );
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::pollValues()
    {
        OpButtonVector<OpButtonWidget *> providers;
        OpButtonVector<OpButtonIntrusivePtr<OpButtonWidget>> retained;
        {
            std::lock_guard<OpButtonMutex> lock( m_providerMutex );
            providers = m_providers;
            for( OpButtonWidget * widget : providers )
            {
                // Layout-owned widgets must survive removal from a provider callback.
                // Widgets without intrusive owners remain borrowed from the caller.
                if( widget->getReferenceCount() == 0 )
                {
                    continue;
                }

                retained.emplace_back( widget );
            }
        }

        for( OpButtonWidget * widget : providers )
        {
            {
                std::lock_guard<OpButtonMutex> lock( m_providerMutex );
                if( std::find( m_providers.begin(), m_providers.end(), widget ) == m_providers.end() )
                {
                    continue;
                }
            }

            if( widget->pollProvider() == false )
            {
                continue;
            }

            ++m_revision;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::incrementRevision()
    {
        m_revision++;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonUI::hasUnsent() const
    {
        return m_revision.load() != m_sentRevision;
    }

    //////////////////////////////////////////////////////////////////////////
    void OpButtonUI::markSent()
    {
        m_sentRevision = m_revision.load();
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonUI::hasTabs() const
    {
        return !m_tabs.empty();
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonUI::getRevision() const
    {
        return m_revision.load();
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonUI::toJson()
    {
        std::lock_guard<OpButtonMutex> lock( m_tabMutex );
        OpButtonJson json = OpButtonJson::object();
        json.put( "version", OpButtonJson( VERSION ) );
        json.put( "revision", OpButtonJson( m_revision.load() ) );

        OpButtonJson tabsArr = OpButtonJson::array();
        for( const auto & tab : m_tabs )
        {
            tabsArr.add( tab->toJson() );
        }
        json.put( "tabs", tabsArr );
        return json;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonUI::CallbackEntry::CallbackEntry()
        : func()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonUI::CallbackEntry::~CallbackEntry()
    {
    }
} // namespace OpButton
