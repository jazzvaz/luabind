// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>

struct base : counted_type<base>
{
    int f()
    {
        return 5;
    }
};

COUNTER_GUARD(base);

int f(int x)
{
    return x + 1;
}

int f(int x, int y)
{
    return x + y;
}

int string_length(luabind::string v)
{
	return v.length();
}

base* create_base()
{
    return luabind::luabind_new<base>();
}

void test_value_converter(const luabind::string str)
{
    CHECK(str == "converted string");
}

void test_pointer_converter(const char* const str)
{
    CHECK(std::strcmp(str, "converted string") == 0);
}

struct copy_me
{
};

void take_by_value(copy_me /*m*/)
{
}

int function_should_never_be_called(lua_State* L)
{
    lua_pushnumber(L, 10);
    return 1;
}

TEST_CASE("free_functions")
{
    using namespace luabind;

    lua_pushcclosure(L, &function_should_never_be_called, 0);
    lua_setglobal(L, "f");

    DOSTRING(L,"assert(f() == 10)");

    module(L)
    [
        class_<copy_me>("copy_me")
            .def(constructor<>()),
    
        class_<base>("base")
            .def("f", &base::f),


        def("by_value", &take_by_value),

        def("f", (int(*)(int)) &f),
        def("f", (int(*)(int, int)) &f),
        def("create", &create_base, policy::adopt<0>()),
		def("string_length", &string_length),
        def("test_value_converter", &test_value_converter),
        def("test_pointer_converter", &test_pointer_converter)
  
    ];

    DOSTRING(L,
        "e = create()\n"
        "assert(e:f() == 5)");

    DOSTRING(L,"assert(f(7) == 8)");

    DOSTRING(L,"assert(f(3, 9) == 12)");

//    DOSTRING(L,"set_functor(function(x) return x * 10 end)");

//    CHECK(functor_test(20) == 200);

//    DOSTRING(L,"set_functor(nil)");

    DOSTRING(L,"function lua_create() return create() end");
    base* ptr = call_function<base*, policy::adopt<0>>(L, "lua_create");
    luabind_delete(ptr);

	int Arg0=1;
	CHECK(call_function<int>(L, "f", Arg0)==2);

	luabind::string Arg1="lua means moon";
	CHECK(call_function<int>(L, "string_length", Arg1)==14);

	double Arg2=2;
	CHECK(call_function<int>(L, "f", Arg2)==3);
	
    DOSTRING(L,"test_value_converter('converted string')");
    DOSTRING(L,"test_pointer_converter('converted string')");

    DOSTRING_EXPECTED(L, "f('incorrect', 'parameters')",
        "No matching overload found, candidates:\n"
        "int f(int,int)\n"
        "int f(int)\n"
		"Passed arguments [2]: string ('incorrect'), string ('parameters')\n");


	DOSTRING(L,"function failing_fun() error('expected error message') end");

	char const* expected_msg =
#if LUA_VERSION_NUM >= 502
	"[string \"function failing_fun() error('expected error ...\"]:1: expected error message";
#else
	"[string \"function failing_fun() error('expected erro...\"]:1: expected error message";
#endif

	CHECK_THROWS_WITH_AS(
		call_function<void>(L, "failing_fun"),
		expected_msg,
		luabind::error const&);
}

