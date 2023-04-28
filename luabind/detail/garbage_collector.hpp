// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

namespace luabind::detail
{
    // function that is used as __gc metafunction on several objects
    template <class T>
    int garbage_collector(lua_State* L)
    {
        auto* obj = static_cast<T*>(lua_touserdata(L, -1));
        obj->~T();
        return 0;
    }

    template <class T>
    struct garbage_collector_s
    {
        static int apply(lua_State* L)
        {
            T* obj = static_cast<T*>(lua_touserdata(L, -1));
            obj->~T();
            return 0;
        }
    };
} // namespace luabind::detail
