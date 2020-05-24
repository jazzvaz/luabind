// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2012 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/make_function.hpp>
#include <luabind/lua_state_fwd.hpp>

namespace luabind
{
	LUABIND_API void set_package_preload(lua_State* L, char const* modulename, object const& loader);

	template <typename F>
	void set_package_preload(lua_State* L, char const* modulename, F loader)
	{
		object f = make_function(L, loader, false);
		// Call add_overload to correctly set the name of f.
		// Inefficiency should not matter here.
		detail::add_overload(luabind::newtable(L), modulename, f);
		set_package_preload(L, modulename, f);
	}
} // namespace luabind
