// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

// std::reference_wrapper...
#include <type_traits>  // std::true_type...
#include <cstring>

namespace luabind::detail
{
    template <class T>
    struct type_ {};

    struct ltstr
    {
        bool operator()(char const* s1, char const* s2) const
        { return std::strcmp(s1, s2) < 0; }
    };

    template <int N>
    struct aligned
    {
        char storage[N];
    };

    // returns the offset added to a Derived* when cast to a Base*
    template <class Derived, class Base>
    ptrdiff_t ptr_offset(type_<Derived>, type_<Base>)
    {
        aligned<sizeof(Derived)> obj;
        auto* derived = reinterpret_cast<Derived*>(&obj);
        auto* base = static_cast<Base*>(derived);
        return ptrdiff_t(reinterpret_cast<char*>(base) - reinterpret_cast<char*>(derived));
    }
} // namespace luabind::detail
