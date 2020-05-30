// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

namespace luabind
{
    // Port of Lua5.2 luaL_tolstring()
    // Based on http://www.lua.org/source/5.2/lauxlib.c.html#luaL_tolstring
    // Converts any Lua value at the given index to a C string in a reasonable format.
    // The resulting string is pushed onto the stack and also returned by the function.
    // If len is not NULL, the function also sets *len with the string length.
    // If the value has a metatable with a "__tostring" field, then luaL_tolstring calls
    // the corresponding metamethod with the value as argument, and uses the result of
    // the call as its result.
    LUABIND_API const char* lua52L_tolstring(lua_State* L, int idx, size_t* len);

    namespace detail
    {
        LUABIND_API const char* luaL_tolstring_diag(lua_State* L, int idx, size_t* len);
    } // namespace detail
} // namespace luabind
