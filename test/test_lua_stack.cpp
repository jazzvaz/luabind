// MIT License
// Copyright (c) 2020 Pavel Kovalenko

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>

struct base : counted_type<base>
{};

COUNTER_GUARD(base);

void test_main(lua_State* L)
{
	using namespace luabind;
	module(L)
	[
		class_<base>("base")
	];
	{
		const int top0 = lua_gettop(L);
		lua_stack::push(L, 1);
		TEST_CHECK(lua_gettop(L) == top0 + 1);
		lua_stack::pop(L);
		const int top1 = lua_gettop(L);
		TEST_CHECK(top0 == top1);
	}
	{
		lua_stack::push(L, 42);
		TEST_CHECK(lua_stack::pop<int>(L) == 42);
	}
	{
		base* b = luabind_new<base>();
		lua_stack::push<policy::adopt<1>>(L, b);
		globals(L)["b"] = object(from_stack(L, -1));
		lua_stack::pop(L);
		DOSTRING(L,
			"b = nil\n"
			"collectgarbage()");
		TEST_CHECK(base::count == 0);
	}
	{
		base* b = luabind_new<base>();
		lua_stack::push<policy::adopt<1>>(L, b);
		globals(L)["b"] = object(from_stack(L, -1));
		base* nb = lua_stack::pop<base*, policy::adopt<1>>(L);
		TEST_CHECK(b == nb);
		DOSTRING(L,
			"b = nil\n"
			"collectgarbage()");
		TEST_CHECK(base::count == 1);
		luabind_delete(b);
	}
}
