#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonTab::OpButtonTab( OpButtonUI & _ui, const OpButtonString & _id )
        : m_ui( _ui )
        , m_title( _id )
        , m_internalId( _ui.nextId() )
        , m_id( _id )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout & OpButtonTab::vertical()
    {
        m_layout = OpButtonIntrusivePtr<OpButtonLayout>( new OpButtonLayout( m_ui, OpButtonLayout::Direction::VERTICAL ) );
        m_ui.incrementRevision();
        return *m_layout;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout & OpButtonTab::horizontal()
    {
        m_layout = OpButtonIntrusivePtr<OpButtonLayout>( new OpButtonLayout( m_ui, OpButtonLayout::Direction::HORIZONTAL ) );
        m_ui.incrementRevision();
        return *m_layout;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout & OpButtonTab::setJson( const OpButtonJson & _json, const OpButtonJsonCallback & _callback )
    {
        OpButtonIntrusivePtr<OpButtonLayout> layout = OpButtonLayout::fromJson( m_ui, _json, _callback );
        m_layout.swap( layout );
        m_ui.incrementRevision();

        return *m_layout;
    }
    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonTab::toJson() const
    {
        OpButtonJson json = OpButtonJson::object();
        json.put( "id", OpButtonJson( m_internalId ) );
        json.put( "title", OpButtonJson( m_title ) );
        if( m_layout != nullptr )
        {
            json.put( "layout", m_layout->toJson() );
        }
        return json;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTab & OpButtonTab::setId( const OpButtonString & _id )
    {
        m_id = _id;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTab & OpButtonTab::setName( const OpButtonString & _name )
    {
        m_title = _name;
        m_ui.incrementRevision();

        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonTab::title() const
    {
        return m_title;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonString & OpButtonTab::id() const
    {
        return m_id;
    }

    //////////////////////////////////////////////////////////////////////////
    int OpButtonTab::internalId() const
    {
        return m_internalId;
    }

    //////////////////////////////////////////////////////////////////////////
    bool OpButtonTab::hasLayout() const
    {
        return m_layout != nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout * OpButtonTab::getLayout() const
    {
        OpButtonLayout * layout = m_layout.get();

        return layout;
    }
    //////////////////////////////////////////////////////////////////////////
    OpButtonTab::~OpButtonTab()
    {
    }

    OpButtonLayout & OpButtonTab::grid()
    {
        m_layout = OpButtonIntrusivePtr<OpButtonLayout>( new OpButtonLayout( m_ui, OpButtonLayout::Direction::GRID ) );
        m_ui.incrementRevision();
        return *m_layout;
    }

} // namespace OpButton
