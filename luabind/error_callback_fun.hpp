// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

// Internal Includes
#include <luabind/config.hpp>
#include <luabind/lua_state_fwd.hpp>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace luabind
{
	class type_id;

	using error_callback_fun       = void(*)(lua_State*);
	using cast_failed_callback_fun = void(*)(lua_State*, type_id const&);
	using pcall_callback_fun       = void(*)(lua_State*);
}
