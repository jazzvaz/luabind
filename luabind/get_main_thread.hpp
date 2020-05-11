// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#ifndef LUABIND_GET_MAIN_THREAD_090321_HPP
# define LUABIND_GET_MAIN_THREAD_090321_HPP

# include <luabind/config.hpp>
# include <luabind/lua_state_fwd.hpp>

namespace luabind {

	LUABIND_API lua_State* get_main_thread(lua_State* L);

} // namespace luabind

#endif // LUABIND_GET_MAIN_THREAD_090321_HPP

