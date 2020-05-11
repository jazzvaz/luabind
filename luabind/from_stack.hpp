// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <luabind/lua_state_fwd.hpp>

namespace luabind {

	struct from_stack
	{
		from_stack(lua_State* interpreter, int index)
			: interpreter(interpreter)
			, index(index)
		{}

		lua_State* interpreter;
		int index;
	};

} // namespace luabind
