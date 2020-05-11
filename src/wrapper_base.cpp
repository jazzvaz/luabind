// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#define LUABIND_BUILDING

#include <luabind/config.hpp>
#include <luabind/lua_include.hpp>
#include <luabind/function.hpp>
#include <luabind/detail/object_rep.hpp>
#include <luabind/detail/class_rep.hpp>
#include <luabind/detail/stack_utils.hpp>

namespace luabind {
	namespace detail {

		LUABIND_API void do_call_member_selection(lua_State* L, char const* name)
		{
			object_rep* obj = static_cast<object_rep*>(lua_touserdata(L, -1));
			assert(obj);

			lua_pushstring(L, name);
			lua_gettable(L, -2);
			lua_replace(L, -2);

			if(!is_luabind_function(L, -1))
				return;

			// this (usually) means the function has not been
			// overridden by lua, call the default implementation
			lua_pop(L, 1);
			obj->crep()->get_default_table(L); // push the crep table
			lua_pushstring(L, name);
			lua_gettable(L, -2);
			lua_remove(L, -2); // remove the crep table
		}
	}
}

