// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/typeid.hpp>

namespace luabind::detail
{
    inline void call_error(lua_State* L)
    {
#ifndef LUABIND_NO_EXCEPTIONS
        throw luabind::error(L);
#else
        error_callback_fun e = get_error_callback();
        if (e)
        {
            e(L);
            if (!get_call_error_break())
                return;
        }
        assert(!"the lua function threw an error and exceptions are disabled."
            " If you want to handle the error you can use luabind::set_error_callback()");
        std::terminate();
#endif
    }

    template <typename T>
    void cast_error(lua_State* L)
    {
#ifndef LUABIND_NO_EXCEPTIONS
        throw cast_failed(L, typeid(T));
#else
        cast_failed_callback_fun e = get_cast_failed_callback();
        if (e)
        {
            e(L, typeid(T));
            if (!get_cast_error_break())
                return;
        }
        assert(!"the lua function's return value could not be converted."
            " If you want to handle the error you can use luabind::set_cast_failed_callback()");
        std::terminate();
#endif
    }

    template <typename... Args>
    void expand_hack(Args... /*args*/) {}
}
