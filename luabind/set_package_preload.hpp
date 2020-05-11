// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2012 The Luabind Authors

#ifndef INCLUDED_set_package_preload_hpp_GUID_563c882e_86f7_4ea7_8603_4594ea41737e
#define INCLUDED_set_package_preload_hpp_GUID_563c882e_86f7_4ea7_8603_4594ea41737e

// Internal Includes
#include <luabind/config.hpp>
#include <luabind/make_function.hpp>
#include <luabind/lua_state_fwd.hpp>

// Library/third-party includes
// - none

// Standard includes
// - none


namespace luabind {

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
}
#endif // INCLUDED_set_package_preload_hpp_GUID_563c882e_86f7_4ea7_8603_4594ea41737e

