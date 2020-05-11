// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#define LUABIND_BUILDING

// Internal Includes
#include <luabind/set_package_preload.hpp>
#include <luabind/config.hpp>           // for LUABIND_API
#include <luabind/detail/object.hpp>    // for object, rawget, globals
#include <luabind/detail/conversion_policies/conversion_policies.hpp>

// Library/third-party includes
#include <luabind/lua_include.hpp>      // for lua_pushstring, lua_rawset, etc

// Standard includes
// - none

static int do_set_package_preload(lua_State* L)
{
	// Note: Use ordinary set/get instead of the raw variants, because this
	// function should not be performance sensitive anyway.
	lua_pushglobaltable(L);
	lua_getfield(L, -1, "package");
	lua_remove(L, -2); // Remove global table.
	lua_getfield(L, -1, "preload");
	lua_remove(L, -2); // Remove package table.
	lua_insert(L, -3); // Move package.preload beneath key and value.
	lua_settable(L, -3); // package.preload[modulename] = loader
	return 0;
}

static int proxy_loader(lua_State* L)
{
	luaL_checkstring(L, 1); // First argument should be the module name.
	lua_settop(L, 1); // Ignore any other arguments.
	lua_pushvalue(L, lua_upvalueindex(1)); // Push the real loader.
	lua_insert(L, 1); // Move it beneath the argument.
	lua_call(L, 1, LUA_MULTRET); // Pops everyhing.
	return lua_gettop(L);
}

namespace luabind {
	LUABIND_API void set_package_preload(lua_State* L, char const* modulename, object const& loader)
	{
		loader.push(L);
		lua_pushcclosure(L, &proxy_loader, 1);
		object const proxy_ldr(from_stack(L, -1));
		lua_pop(L, 1); // pop do_load.
		lua_pushcfunction(L, &do_set_package_preload);
		// Must use object for correct popping in case of errors:
		object do_set(from_stack(L, -1));
		lua_pop(L, 1);
		do_set(modulename, proxy_ldr);
	}

} // namespace luabind

