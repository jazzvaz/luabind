// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#define LUABIND_BUILDING

#include <luabind/lua_include.hpp>       // for lua_gettop, lua_touserdata, etc
#include <luabind/detail/class_rep.hpp>  // for class_rep, is_class_rep
#include <luabind/detail/object_rep.hpp> // for get_instance, object_rep
#include <string>                        // for string

namespace luabind::detail
{
	luabind::string stack_content_by_name(lua_State* L, int start_index)
	{
		luabind::string ret;
		int top = lua_gettop(L);
		for (int i = start_index; i <= top; i++)
		{
			object_rep* obj = get_instance(L, i);
			class_rep* crep = is_class_rep(L, i) ? (class_rep*)lua_touserdata(L, i) : nullptr;
			if (!obj && !crep)
			{
				int type = lua_type(L, i);
				ret += lua_typename(L, type);
			}
			else if (obj)
			{
				if (obj->is_const())
					ret += "const ";
				ret += obj->crep()->name();
			}
			else if (crep)
			{
				ret += "<";
				ret += crep->name();
				ret += ">";
			}
			if (i < top)
				ret += ", ";
		}
		return ret;
	}
} // namespace luabind::detail
