// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct test_class : counted_type<test_class>
{
	test_class() {}
	int f() const
	{
		return 5;
	}
};

int f(int /*a*/)
{
	return 9;
}

int j(lua_State* L)
{
	lua_pushnumber(L, 9);
	return lua_yield(L, 1);
}

void f() {}

COUNTER_GUARD(test_class);

void test_main(lua_State* L)
{
	using namespace luabind;

	module(L)
	[
		class_<test_class>("test")
			.def(constructor<>())
			.def("f", &test_class::f, policy::yield())
	];

	DOSTRING(L,
		"function h()\n"
		"	coroutine.yield(4)"
		"end");

	{
		lua_State* thread = lua_newthread(L);
        TEST_CHECK(resume_function<int>(thread, "h") == 4);
        lua_pop(L, 1); // pop thread
    }

	DOSTRING(L,
		"function g(str)\n"
		"	assert(str == 'foobar')\n"
		"	a = test()"
		"	for i = 1, 10 do\n"
		"		assert(a:f() == i + 4)\n"
		"	end\n"
		"end");

	{
        lua_State* thread = lua_newthread(L);

        TEST_CHECK(resume_function<int>(thread, "g", "foobar") == 5);
		for (int i = 1; i < 10; ++i)
		{
			TEST_CHECK(resume<int>(thread, i + 4) == 5);
		}
		lua_pop(L, 1); // pop thread
	}


	{
        lua_State* thread = lua_newthread(L);
		object g = globals(thread)["g"];

        TEST_CHECK(resume_function<int>(g, "foobar") == 5);
		for (int i = 1; i < 10; ++i)
		{
			TEST_CHECK(resume<int>(thread, i + 4) == 5);
		}
		lua_pop(L, 1); // pop thread
	}

}

