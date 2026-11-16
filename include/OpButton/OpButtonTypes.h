#pragma once

#include "OpButton/OpButtonStlAllocator.h"

#include "OpButton/OpButtonIntrusivePtr.h"
#include "OpButton/OpButtonRefCounted.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace OpButton
{
    //////////////////////////////////////////////////////////////////////////
    // Container types with custom allocator
    //////////////////////////////////////////////////////////////////////////

    using OpButtonChar = char;
    using OpButtonString = std::basic_string<OpButtonChar, std::char_traits<OpButtonChar>, OpButtonStlAllocator<OpButtonChar>>;

    template<class T, class A = OpButtonStlAllocator<T>>
    using OpButtonVector = std::vector<T, A>;

    template<class K, class T, class L = std::less<K>, class A = OpButtonStlAllocator<std::pair<const K, T>>>
    using OpButtonMap = std::map<K, T, L, A>;

    template<class K, class T, class H = std::hash<K>, class E = std::equal_to<K>, class A = OpButtonStlAllocator<std::pair<const K, T>>>
    using OpButtonUnorderedMap = std::unordered_map<K, T, H, E, A>;

    //////////////////////////////////////////////////////////////////////////
    // Function wrapper
    //////////////////////////////////////////////////////////////////////////

    template<class T>
    using OpButtonFunction = std::function<T>;

    //////////////////////////////////////////////////////////////////////////
    // Utility types
    //////////////////////////////////////////////////////////////////////////

    template<class T1, class T2>
    using OpButtonPair = std::pair<T1, T2>;

    //////////////////////////////////////////////////////////////////////////
    // Synchronization types
    //////////////////////////////////////////////////////////////////////////

    using OpButtonMutex = std::mutex;

    template<class T>
    using OpButtonAtomic = std::atomic<T>;

    using OpButtonThread = std::thread;
    using OpButtonConditionVariable = std::condition_variable;

    //////////////////////////////////////////////////////////////////////////
    // Helpers
    //////////////////////////////////////////////////////////////////////////

    template<class T>
    OpButtonString opbuttonToString( T _value )
    {
        std::string value = std::to_string( _value );
        OpButtonString result( value.data(), value.size() );
        return result;
    }

} // namespace OpButton
