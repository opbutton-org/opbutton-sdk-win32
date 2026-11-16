#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace OpButton
{
    class OpButtonRefCounted
    {
    public:
        OpButtonRefCounted();
        virtual ~OpButtonRefCounted();
        OpButtonRefCounted( const OpButtonRefCounted & ) = delete;
        OpButtonRefCounted & operator=( const OpButtonRefCounted & ) = delete;

        void addRef() const noexcept;
        void release() const noexcept;
        uint32_t getReferenceCount() const noexcept;

        static void * operator new( size_t _size );
        static void operator delete( void * _memory ) noexcept;

    private:
        mutable std::atomic<uint32_t> m_references;
    };
} // namespace OpButton
