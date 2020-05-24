// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#define LUABIND_BUILDING

#include <luabind/lua_include.hpp>
#include <luabind/luabind.hpp>

namespace luabind::detail
{
	// expects two tables on the lua stack:
	// 1: destination
	// 2: source
	static void copy_member_table(lua_State* L)
	{
		lua_pushnil(L);
		while (lua_next(L, -2))
		{
			lua_pushliteral(L, "__init");
			if (lua_compare(L, -1, -3, LUA_OPEQ))
			{
				lua_pop(L, 2);
				continue;
			}
			else
				lua_pop(L, 1); // __init string
			lua_pushliteral(L, "__finalize");
			if (lua_compare(L, -1, -3, LUA_OPEQ))
			{
				lua_pop(L, 2);
				continue;
			}
			else
				lua_pop(L, 1); // __finalize string
			lua_pushvalue(L, -2); // copy key
			lua_insert(L, -2);
			lua_settable(L, -5);
		}
	}

	int create_class::stage2(lua_State* L)
	{
		class_rep* crep = static_cast<class_rep*>(lua_touserdata(L, lua_upvalueindex(1)));
		assert(crep != nullptr && "internal error, please report");
		assert(is_class_rep(L, lua_upvalueindex(1)) && "internal error, please report");
#ifndef LUABIND_NO_ERROR_CHECKING
		if (!is_class_rep(L, 1))
		{
			lua_pushliteral(L, "expected class to derive from or a newline");
			lua_error(L);
		}
#endif
		class_rep* base = static_cast<class_rep*>(lua_touserdata(L, 1));
		crep->add_base_class(base);
		// copy base class members
		crep->get_table(L);
		base->get_table(L);
		copy_member_table(L);
		crep->get_default_table(L);
		base->get_default_table(L);
		copy_member_table(L);
		crep->set_type(base->type());
		return 0;
	}

	int create_class::stage1(lua_State* L)
	{
#ifndef LUABIND_NO_ERROR_CHECKING
		if (lua_gettop(L) != 1 || lua_type(L, 1) != LUA_TSTRING || lua_isnumber(L, 1))
		{
			lua_pushliteral(L, "invalid construct, expected class name");
			lua_error(L);
		}
		if (std::strlen(lua_tostring(L, 1)) != lua_rawlen(L, 1))
		{
			lua_pushliteral(L, "luabind does not support class names with extra nulls");
			lua_error(L);
		}
#endif
		char const* name = lua_tostring(L, 1);
		void* c = lua_newuserdata(L, sizeof(class_rep));
		new (c) class_rep(L, name);
		// make the class globally available
		lua_pushvalue(L, -1);
		lua_setglobal(L, name);
		// also add it to the closure as return value
		lua_pushcclosure(L, &stage2, 1);
		return 1;
	}
} // namespace luabind::detail
