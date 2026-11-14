#pragma once

#include "OpButton/OpButtonStlAllocator.h"

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <utility>

namespace OpButton {

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
// Smart pointer types
//////////////////////////////////////////////////////////////////////////

template<class T, class D = std::default_delete<T>>
using OpButtonUniquePtr = std::unique_ptr<T, D>;

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
inline OpButtonString opbuttonToString(T val) {
    auto s = std::to_string(val);
    return OpButtonString(s.data(), s.size());
}

} // namespace OpButton
