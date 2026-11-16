#pragma once
#include <cstddef>

namespace OpButton
{
    template<class T>
    class OpButtonIntrusivePtr
    {
    public:
        OpButtonIntrusivePtr() noexcept;
        ~OpButtonIntrusivePtr();
        OpButtonIntrusivePtr( std::nullptr_t ) noexcept;
        explicit OpButtonIntrusivePtr( T * _pointer ) noexcept;
        OpButtonIntrusivePtr( const OpButtonIntrusivePtr & _other ) noexcept;
        OpButtonIntrusivePtr( OpButtonIntrusivePtr && _other ) noexcept;
        template<class U>
        OpButtonIntrusivePtr( const OpButtonIntrusivePtr<U> & _other ) noexcept;
        OpButtonIntrusivePtr & operator=( const OpButtonIntrusivePtr & _other ) noexcept;
        OpButtonIntrusivePtr & operator=( OpButtonIntrusivePtr && _other ) noexcept;
        OpButtonIntrusivePtr & operator=( std::nullptr_t ) noexcept;

        T * get() const noexcept;
        T * operator->() const noexcept;
        T & operator*() const noexcept;
        explicit operator bool() const noexcept;
        bool operator==( std::nullptr_t ) const noexcept;
        bool operator!=( std::nullptr_t ) const noexcept;

        void reset( T * _pointer = nullptr ) noexcept;
        void swap( OpButtonIntrusivePtr & _other ) noexcept;

    private:
        T * m_pointer;
    };
} // namespace OpButton

#include "OpButton/OpButtonIntrusivePtr.inl"
