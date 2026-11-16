#include "OpButton/OpButtonLogAttributeBuilder.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogAttributeBuilder & OpButtonLogAttributeBuilder::put( const OpButtonString & _key, const OpButtonString & _value )
    {
        m_staticAttributes[_key] = _value;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogAttributeBuilder & OpButtonLogAttributeBuilder::putProvider( const OpButtonString & _key, OpButtonValueProvider * _provider )
    {
        m_dynamicAttributes[_key] = _provider;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonMap<OpButtonString, OpButtonString> & OpButtonLogAttributeBuilder::staticAttributes() const
    {
        return m_staticAttributes;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonMap<OpButtonString, OpButtonValueProvider *> & OpButtonLogAttributeBuilder::dynamicAttributes() const
    {
        return m_dynamicAttributes;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogAttributeBuilder::OpButtonLogAttributeBuilder()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonLogAttributeBuilder::~OpButtonLogAttributeBuilder()
    {
    }
} // namespace OpButton
