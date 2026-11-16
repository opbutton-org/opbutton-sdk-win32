#include "OpButton/OpButtonSessionAttributeBuilder.h"

namespace OpButton
{

    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionAttributeBuilder & OpButtonSessionAttributeBuilder::put( const OpButtonString & _key, const OpButtonJson & _value )
    {
        m_staticAttributes[_key] = _value;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionAttributeBuilder & OpButtonSessionAttributeBuilder::putProvider( const OpButtonString & _key, OpButtonValueProvider * _provider )
    {
        m_dynamicAttributes[_key] = _provider;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonMap<OpButtonString, OpButtonJson> & OpButtonSessionAttributeBuilder::staticAttributes() const
    {
        return m_staticAttributes;
    }

    //////////////////////////////////////////////////////////////////////////
    const OpButtonMap<OpButtonString, OpButtonValueProvider *> & OpButtonSessionAttributeBuilder::dynamicAttributes() const
    {
        return m_dynamicAttributes;
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionAttributeBuilder::OpButtonSessionAttributeBuilder()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    OpButtonSessionAttributeBuilder::~OpButtonSessionAttributeBuilder()
    {
    }
} // namespace OpButton
