// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#ifndef LUA_INCLUDE_HPP_INCLUDED
#define LUA_INCLUDE_HPP_INCLUDED

#ifndef LUABIND_CPLUSPLUS_LUA
extern "C"
{
#endif

#include "lua.h"
#include "lauxlib.h"

#ifndef LUABIND_CPLUSPLUS_LUA
}
#endif

#if LUA_VERSION_NUM < 502
# include <luabind/lua_extensions.hpp>
# define lua_compare(L, index1, index2, fn) fn(L, index1, index2)
# define LUA_OPEQ lua_equal
# define LUA_OPLT lua_lessthan
# define lua_rawlen lua_objlen
# define lua_pushglobaltable(L) lua_pushvalue(L, LUA_GLOBALSINDEX)
# define lua_getuservalue lua_getfenv
# define lua_setuservalue lua_setfenv
# define luaL_tolstring luabind::lua52L_tolstring
# define LUA_OK 0

inline int lua_absindex(lua_State* L, int i)
{
	if (i < 0 && i > LUA_REGISTRYINDEX)
		i += lua_gettop(L) + 1;
	return i;
}

inline void lua_rawsetp(lua_State* L, int i, void* p)
{
	int abs_i = lua_absindex(L, i);
	luaL_checkstack(L, 1, "not enough stack slots");
	lua_pushlightuserdata(L, p);
	lua_insert(L, -2);
	lua_rawset(L, abs_i);
}

inline void lua_rawgetp(lua_State* L, int i, void* p)
{
	int abs_i = lua_absindex(L, i);
	lua_pushlightuserdata(L, p);
	lua_rawget(L, abs_i);
}
#endif

#endif

