// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include <luabind/detail/pcall.hpp>
#include <luabind/error.hpp>
#include <luabind/lua_include.hpp>

namespace luabind::detail
{
    int pcall(lua_State* L, int nargs, int nresults)
    {
        pcall_callback_fun e = get_pcall_callback();
        int en = 0;
        if (e)
        {
            int base = lua_gettop(L) - nargs;
            e(L);
            lua_insert(L, base); // push pcall_callback under chunk and args
            en = base;
        }
        int result = lua_pcall(L, nargs, nresults, en);
        if (en)
            lua_remove(L, en); // remove pcall_callback
        return result;
    }

    int resume_impl(lua_State* L, int nargs, int)
    {
#if LUA_VERSION_NUM >= 502
        int res = lua_resume(L, NULL, nargs);
#else
        int res = lua_resume(L, nargs);
#endif
        // Lua 5.1 added  LUA_YIELD as a possible return value,
        // this was causing crashes, because the caller expects 0 on success.
        return res == LUA_YIELD ? 0 : res;
    }
} // namespace luabind::detail
