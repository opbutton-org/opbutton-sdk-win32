#include "OpButton/OpButtonLayout.h"
#include "OpButton/OpButtonButton.h"
#include "OpButton/OpButtonInput.h"
#include "OpButton/OpButtonLabel.h"
#include "OpButton/OpButtonLogs.h"
#include "OpButton/OpButtonSelect.h"
#include "OpButton/OpButtonStat.h"
#include "OpButton/OpButtonSwitch.h"
#include "OpButton/OpButtonTable.h"
#include "OpButton/OpButtonTextarea.h"
#include "OpButton/OpButtonUI.h"

namespace OpButton
{

    class OpButtonLayout::PaddingElement : public OpButtonLayoutChild
    {
    public:
        PaddingElement( double _weight )
            : m_weight( _weight )
        {
        }

        ~PaddingElement() override
        {
        }

        OpButtonJson toJson() const override
        {
            return OpButtonJson::object().put( "type", OpButtonJson( "padding" ) ).put( "weight", OpButtonJson( m_weight ) );
        }

    private:
        double m_weight;
    };

    class OpButtonLayout::CollapsibleElement : public OpButtonLayoutChild
    {
    public:
        CollapsibleElement( OpButtonUI & _ui, const OpButtonString & _title )
            : m_title( _title )
            , m_content( OpButtonIntrusivePtr<OpButtonLayout>( new OpButtonLayout( _ui, Direction::VERTICAL ) ) )
        {
        }

        ~CollapsibleElement() override
        {
        }

        OpButtonLayout & content()
        {
            return *m_content;
        }

        OpButtonJson toJson() const override
        {
            OpButtonJson contentJson = m_content->toJson();
            return OpButtonJson::object().put( "type", OpButtonJson( "collapsible" ) ).put( "title", OpButtonJson( m_title ) ).put( "children", contentJson.optArray( "children" ) );
        }

    private:
        OpButtonString m_title;
        OpButtonIntrusivePtr<OpButtonLayout> m_content;
    };

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout::OpButtonLayout( OpButtonUI & _ui, Direction _dir )
        : m_ui( _ui )
        , m_direction( _dir )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout & OpButtonLayout::addVertical()
    {
        OpButtonIntrusivePtr<OpButtonLayout> sub = OpButtonIntrusivePtr<OpButtonLayout>( new OpButtonLayout( m_ui, Direction::VERTICAL ) );
        OpButtonLayout * ptr = sub.get();
        m_children.push_back( std::move( sub ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout & OpButtonLayout::addHorizontal()
    {
        OpButtonIntrusivePtr<OpButtonLayout> sub = OpButtonIntrusivePtr<OpButtonLayout>( new OpButtonLayout( m_ui, Direction::HORIZONTAL ) );
        OpButtonLayout * ptr = sub.get();
        m_children.push_back( std::move( sub ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout & OpButtonLayout::addCollapsible( const OpButtonString & _title )
    {
        OpButtonIntrusivePtr<CollapsibleElement> collapsible = OpButtonIntrusivePtr<CollapsibleElement>( new CollapsibleElement( m_ui, _title ) );
        OpButtonLayout & content = collapsible->content();
        m_children.push_back( std::move( collapsible ) );
        m_ui.incrementRevision();
        return content;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout & OpButtonLayout::addPadding( double _weight )
    {
        m_children.push_back( OpButtonIntrusivePtr<PaddingElement>( new PaddingElement( _weight ) ) );
        m_ui.incrementRevision();
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonButton & OpButtonLayout::addButton( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonButton> w = OpButtonIntrusivePtr<OpButtonButton>( new OpButtonButton( m_ui, _label ) );
        OpButtonButton * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLabel & OpButtonLayout::addLabel( const OpButtonString & _text )
    {
        OpButtonIntrusivePtr<OpButtonLabel> w = OpButtonIntrusivePtr<OpButtonLabel>( new OpButtonLabel( m_ui, _text ) );
        OpButtonLabel * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonStat & OpButtonLayout::addStat( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonStat> w = OpButtonIntrusivePtr<OpButtonStat>( new OpButtonStat( m_ui, _label ) );
        OpButtonStat * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSwitch & OpButtonLayout::addSwitch( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonSwitch> w = OpButtonIntrusivePtr<OpButtonSwitch>( new OpButtonSwitch( m_ui, _label ) );
        OpButtonSwitch * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonInput & OpButtonLayout::addInput( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonInput> w = OpButtonIntrusivePtr<OpButtonInput>( new OpButtonInput( m_ui, _label ) );
        OpButtonInput * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSelect & OpButtonLayout::addSelect( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonSelect> w = OpButtonIntrusivePtr<OpButtonSelect>( new OpButtonSelect( m_ui, _label ) );
        OpButtonSelect * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTextarea & OpButtonLayout::addTextarea( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonTextarea> w = OpButtonIntrusivePtr<OpButtonTextarea>( new OpButtonTextarea( m_ui, _label ) );
        OpButtonTextarea * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonTable & OpButtonLayout::addTable( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonTable> w = OpButtonIntrusivePtr<OpButtonTable>( new OpButtonTable( m_ui, _label ) );
        OpButtonTable * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogs & OpButtonLayout::addLogs( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonLogs> w = OpButtonIntrusivePtr<OpButtonLogs>( new OpButtonLogs( m_ui, _label ) );
        OpButtonLogs * ptr = w.get();
        m_children.push_back( std::move( w ) );
        m_ui.incrementRevision();
        return *ptr;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonJson OpButtonLayout::toJson() const
    {
        OpButtonJson json = OpButtonJson::object();
        json.put( "type", OpButtonJson( "layout" ) );
        json.put( "direction", OpButtonJson( m_direction == Direction::VERTICAL ? "vertical" : "horizontal" ) );

        OpButtonJson childrenArr = OpButtonJson::array();
        for( const auto & child : m_children )
        {
            childrenArr.add( child->toJson() );
        }
        json.put( "children", childrenArr );
        return json;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout::~OpButtonLayout()
    {
    }
} // namespace OpButton
