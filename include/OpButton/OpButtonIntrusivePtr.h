#pragma once
#include <cstddef>
#include <cassert>
#include <utility>

namespace OpButton
{
    template<class T>
    class OpButtonIntrusivePtr
    {
    public:
        OpButtonIntrusivePtr() noexcept;
        OpButtonIntrusivePtr( std::nullptr_t ) noexcept;
        explicit OpButtonIntrusivePtr( T * _pointer ) noexcept;
        OpButtonIntrusivePtr( const OpButtonIntrusivePtr & _other ) noexcept;
        OpButtonIntrusivePtr( OpButtonIntrusivePtr && _other ) noexcept;

        template<class U>
        OpButtonIntrusivePtr( const OpButtonIntrusivePtr<U> & _other ) noexcept;

        ~OpButtonIntrusivePtr();

    public:
        OpButtonIntrusivePtr & operator=( const OpButtonIntrusivePtr & _other ) noexcept;
        OpButtonIntrusivePtr & operator=( OpButtonIntrusivePtr && _other ) noexcept;
        OpButtonIntrusivePtr & operator=( std::nullptr_t ) noexcept;
        T * operator->() const noexcept;
        T & operator*() const noexcept;
        explicit operator bool() const noexcept;
        bool operator==( std::nullptr_t ) const noexcept;
        bool operator!=( std::nullptr_t ) const noexcept;

    public:
        T * get() const noexcept;
        void reset( T * _pointer = nullptr ) noexcept;
        void swap( OpButtonIntrusivePtr & _other ) noexcept;

    private:
        T * m_pointer;
    };

    template<class T>
    OpButtonIntrusivePtr<T>::OpButtonIntrusivePtr() noexcept
        : m_pointer( nullptr )
    {
    }

    template<class T>
    OpButtonIntrusivePtr<T>::OpButtonIntrusivePtr( std::nullptr_t ) noexcept
        : m_pointer( nullptr )
    {
    }

    template<class T>
    OpButtonIntrusivePtr<T>::OpButtonIntrusivePtr( T * _pointer ) noexcept
        : m_pointer( _pointer )
    {
        if( m_pointer != nullptr )
        {
            m_pointer->addRef();
        }
    }

    template<class T>
    OpButtonIntrusivePtr<T>::OpButtonIntrusivePtr( const OpButtonIntrusivePtr & _other ) noexcept
        : OpButtonIntrusivePtr( _other.m_pointer )
    {
    }

    template<class T>
    OpButtonIntrusivePtr<T>::OpButtonIntrusivePtr( OpButtonIntrusivePtr && _other ) noexcept
        : m_pointer( _other.m_pointer )
    {
        _other.m_pointer = nullptr;
    }

    template<class T>
    OpButtonIntrusivePtr<T>::~OpButtonIntrusivePtr()
    {
        if( m_pointer != nullptr )
        {
            m_pointer->release();
        }
    }

    template<class T>
    OpButtonIntrusivePtr<T> & OpButtonIntrusivePtr<T>::operator=( const OpButtonIntrusivePtr & _other ) noexcept
    {
        this->reset( _other.m_pointer );
        return *this;
    }

    template<class T>
    OpButtonIntrusivePtr<T> & OpButtonIntrusivePtr<T>::operator=( OpButtonIntrusivePtr && _other ) noexcept
    {
        OpButtonIntrusivePtr other( std::move( _other ) );
        this->swap( other );
        return *this;
    }

    template<class T>
    OpButtonIntrusivePtr<T> & OpButtonIntrusivePtr<T>::operator=( std::nullptr_t ) noexcept
    {
        this->reset();
        return *this;
    }

    template<class T>
    T * OpButtonIntrusivePtr<T>::get() const noexcept
    {
        return m_pointer;
    }

    template<class T>
    T * OpButtonIntrusivePtr<T>::operator->() const noexcept
    {
        assert( m_pointer != nullptr );
        return m_pointer;
    }

    template<class T>
    T & OpButtonIntrusivePtr<T>::operator*() const noexcept
    {
        assert( m_pointer != nullptr );
        return *m_pointer;
    }

    template<class T>
    OpButtonIntrusivePtr<T>::operator bool() const noexcept
    {
        if( m_pointer == nullptr )
        {
            return false;
        }
        return true;
    }

    template<class T>
    bool OpButtonIntrusivePtr<T>::operator==( std::nullptr_t ) const noexcept
    {
        if( m_pointer != nullptr )
        {
            return false;
        }
        return true;
    }

    template<class T>
    bool OpButtonIntrusivePtr<T>::operator!=( std::nullptr_t ) const noexcept
    {
        if( m_pointer == nullptr )
        {
            return false;
        }
        return true;
    }

    template<class T>
    void OpButtonIntrusivePtr<T>::reset( T * _pointer ) noexcept
    {
        if( _pointer != nullptr )
        {
            _pointer->addRef();
        }
        T * previous = m_pointer;
        m_pointer = _pointer;
        if( previous != nullptr )
        {
            previous->release();
        }
    }

    template<class T>
    void OpButtonIntrusivePtr<T>::swap( OpButtonIntrusivePtr & _other ) noexcept
    {
        std::swap( m_pointer, _other.m_pointer );
    }

    template<class T>
    template<class U>
    OpButtonIntrusivePtr<T>::OpButtonIntrusivePtr( const OpButtonIntrusivePtr<U> & _other ) noexcept
        : OpButtonIntrusivePtr( _other.get() )
    {
    }
} // namespace OpButton
