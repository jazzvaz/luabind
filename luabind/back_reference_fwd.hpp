// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#pragma once

#include <luabind/lua_state_fwd.hpp>

namespace luabind {

	template<class T>
	bool get_back_reference(lua_State* L, T const& x);

	template<class T>
	bool move_back_reference(lua_State* L, T const& x);

} // namespace luabind
