#include "OpButton/OpButtonLayout.h"
#include "OpButtonJsonElement.h"
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
#include "OpButton/OpButtonRadioGroup.h"
#include "OpButton/OpButtonSlider.h"
#include <stdexcept>

namespace OpButton
{

    class OpButtonLayout::PaddingElement
        : public OpButtonLayoutChild
    {
    public:
        PaddingElement( OpButtonUI & _ui, double _weight )
            : OpButtonLayoutChild( _ui )
            , m_weight( _weight )
        {
        }

        ~PaddingElement() override
        {
        }

    public:
        OpButtonJson toJson() const override
        {
            return this->decorate_( OpButtonJson::object().put( "type", OpButtonJson( "padding" ) ).put( "weight", OpButtonJson( m_weight ) ) );
        }

    private:
        double m_weight;
    };

    class OpButtonLayout::CollapsibleElement
        : public OpButtonLayoutChild
    {
    public:
        CollapsibleElement( OpButtonUI & _ui, const OpButtonString & _title )
            : OpButtonLayoutChild( _ui )
            , m_title( _title )
            , m_content( OpButtonIntrusivePtr<OpButtonLayout>( new OpButtonLayout( _ui, Direction::VERTICAL ) ) )
        {
        }

        ~CollapsibleElement() override
        {
        }

    public:
        OpButtonLayout & content()
        {
            return *m_content;
        }

        OpButtonJson toJson() const override
        {
            OpButtonJson contentJson = m_content->toJson();
            return this->decorate_( OpButtonJson::object().put( "type", OpButtonJson( "collapsible" ) ).put( "title", OpButtonJson( m_title ) ).put( "children", OpButtonJson::array().add( contentJson ) ) );
        }

    private:
        OpButtonString m_title;
        OpButtonIntrusivePtr<OpButtonLayout> m_content;
    };

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout::OpButtonLayout( OpButtonUI & _ui, Direction _dir )
        : OpButtonLayoutChild( _ui )
        , m_settings( OpButtonJson::object() )
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
        m_children.push_back( OpButtonIntrusivePtr<PaddingElement>( new PaddingElement( m_ui, _weight ) ) );
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
        OpButtonJson json = m_settings;
        json.put( "type", OpButtonJson( "layout" ) );
        json.put( "direction", OpButtonJson( m_direction == Direction::VERTICAL ? "vertical" : m_direction == Direction::HORIZONTAL ? "horizontal" : "grid" ) );

        OpButtonJson childrenArr = OpButtonJson::array();
        for( const auto & child : m_children )
        {
            childrenArr.add( child->toJson() );
        }
        json.put( "children", childrenArr );
        return this->decorate_( json );
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLayout::~OpButtonLayout()
    {
    }

    OpButtonIntrusivePtr<OpButtonLayout> OpButtonLayout::fromJson( OpButtonUI & _ui, const OpButtonJson & _json, const OpButtonJsonCallback & _callback )
    {
        if( _json.optString( "type" ) != "layout" )
        {
            throw std::invalid_argument( "OpButton JSON root must be a layout" );
        }

        OpButtonString direction = _json.optString( "direction" );
        Direction type;

        if( direction == "vertical" )
        {
            type = Direction::VERTICAL;
        }
        else if( direction == "horizontal" )
        {
            type = Direction::HORIZONTAL;
        }
        else if( direction == "grid" )
        {
            type = Direction::GRID;
        }
        else
        {
            throw std::invalid_argument( "Invalid OpButton JSON layout direction" );
        }

        OpButtonIntrusivePtr<OpButtonLayout> layout( new OpButtonLayout( _ui, type ) );
        for( const auto & entry : _json.objectEntries() )
        {
            if( entry.first == "children" || entry.first == "id" || entry.first == "placement" )
            {
                continue;
            }

            layout->m_settings.put( entry.first, entry.second );
        }
        layout->loadPlacement_( _json );
        OpButtonJson children = _json.get( "children" );

        if( _json.has( "children" ) == true && children.type() != OpButtonJson::TYPE_ARRAY )
        {
            throw std::invalid_argument( "OpButton JSON children must be an array" );
        }

        for( int index = 0; index != children.length(); ++index )
        {
            OpButtonIntrusivePtr<OpButtonJsonElement> child( new OpButtonJsonElement( _ui, children.at( index ) ) );
            layout->m_children.push_back( child );
        }

        for( const OpButtonIntrusivePtr<OpButtonLayoutChild> & child : layout->m_children )
        {
            static_cast<OpButtonJsonElement *>( child.get() )->attach( _callback );
        }

        return layout;
    }

    OpButtonLayout & OpButtonLayout::setting_( const OpButtonString & _key, const OpButtonJson & _value )
    {
        m_settings.put( _key, _value ); m_ui.incrementRevision(); return *this;
    }
    OpButtonLayout & OpButtonLayout::gap( double _pixels ) { return this->setting_( "gap", OpButtonJson( dimension_( _pixels ) ) ); }
    OpButtonLayout & OpButtonLayout::padding( double _pixels ) { return this->setting_( "padding", OpButtonJson( dimension_( _pixels ) ) ); }
    OpButtonLayout & OpButtonLayout::align( const OpButtonString & _value ) { return this->setting_( "align", OpButtonJson( alignment_( _value ) ) ); }
    OpButtonLayout & OpButtonLayout::justify( const OpButtonString & _value )
    {
        if( _value != "start" && _value != "center" && _value != "end" && _value != "space-between" ) { throw std::invalid_argument( "Invalid justification" ); }
        return this->setting_( "justify", OpButtonJson( _value ) );
    }
    OpButtonLayout & OpButtonLayout::wrap( bool _value ) { return this->setting_( "wrap", OpButtonJson( _value ) ); }
    OpButtonLayout & OpButtonLayout::density( const OpButtonString & _value )
    {
        if( _value != "compact" && _value != "comfortable" ) { throw std::invalid_argument( "Invalid density" ); }
        return this->setting_( "density", OpButtonJson( _value ) );
    }
    OpButtonLayout & OpButtonLayout::columns( int _value )
    {
        if( _value < 1 || _value > 12 ) { throw std::invalid_argument( "Invalid column count" ); }
        return this->setting_( "gridColumns", OpButtonJson( _value ) );
    }
    OpButtonLayout & OpButtonLayout::minCellWidth( double _pixels ) { return this->setting_( "minCellWidth", OpButtonJson( dimension_( _pixels, true ) ) ); }
    OpButtonLayout & OpButtonLayout::title( const OpButtonString & _value ) { return this->setting_( "title", OpButtonJson( _value ) ); }
    OpButtonLayout & OpButtonLayout::addGrid()
    {
        OpButtonIntrusivePtr<OpButtonLayout> child( new OpButtonLayout( m_ui, Direction::GRID ) );
        OpButtonLayout * result = child.get(); m_children.push_back( std::move( child ) ); m_ui.incrementRevision(); return *result;
    }
    OpButtonLayout & OpButtonLayout::addGroup( const OpButtonString & _title ) { return this->addVertical().title( _title ); }

    OpButtonRadioGroup & OpButtonLayout::addRadioGroup( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonRadioGroup> child( new OpButtonRadioGroup( m_ui, _label ) );
        OpButtonRadioGroup * result = child.get(); m_children.push_back( std::move( child ) ); m_ui.incrementRevision(); return *result;
    }

    OpButtonSlider & OpButtonLayout::addSlider( const OpButtonString & _label )
    {
        OpButtonIntrusivePtr<OpButtonSlider> child( new OpButtonSlider( m_ui, _label ) );
        OpButtonSlider * result = child.get(); m_children.push_back( std::move( child ) ); m_ui.incrementRevision(); return *result;
    }
} // namespace OpButton
