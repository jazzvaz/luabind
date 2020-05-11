// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#ifndef LUABIND_BACK_REFERENCE_FWD_040510_HPP
#define LUABIND_BACK_REFERENCE_FWD_040510_HPP

#include <luabind/lua_state_fwd.hpp>

namespace luabind {

	template<class T>
	bool get_back_reference(lua_State* L, T const& x);

	template<class T>
	bool move_back_reference(lua_State* L, T const& x);

} // namespace luabind

#endif // LUABIND_BACK_REFERENCE_FWD_040510_HPP

