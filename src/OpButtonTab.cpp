#include "OpButton/OpButtonTab.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonTab::OpButtonTab( OpButtonUI & _ui, const OpButtonString & _title )
        : m_ui( _ui )
        , m_title( _title )
        , m_internalId( _ui.nextId() )
        , m_id( OpButtonString( "tab-" ) + opbuttonToString( m_internalId ) )
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
