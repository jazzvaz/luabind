// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/function_introspection.hpp>
#include <luabind/set_package_preload.hpp>

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


TEST_CASE("function_introspection")
{
    using namespace luabind;
    bind_function_introspection(L);
    // second call must succeed too
    bind_function_introspection(L);
    
    DOSTRING(L,
        "assert(function_info.get_function_name)");
    DOSTRING(L,
        "assert(function_info.get_function_overloads)");

    module(L)
    [
        def("f", (int(*)(int)) &f)
    ];
    
    
    DOSTRING(L,
        "assert(function_info.get_function_name(f) == 'f')");
    
    DOSTRING(L,
        "assert(function_info.get_function_name(x) == '')");
        
    
    DOSTRING(L,
        "for _,v in ipairs(function_info.get_function_overloads(f)) do print(v) end");
    
    DOSTRING(L,
        "assert(#(function_info.get_function_overloads(f)) == 1)");
    /*
        def("f", (int(*)(int, int)) &f),
        def("create", &create_base, adopt(return_value))
//        def("set_functor", &set_functor)
            
#if !(BOOST_MSVC < 1300)
        ,
        def("test_value_converter", &test_value_converter),
        def("test_pointer_converter", &test_pointer_converter)
#endif
            
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
    base* ptr = call_function<base*>(L, "lua_create") [ adopt(result) ];
    luabind_delete(ptr);

#if !(BOOST_MSVC < 1300)
    DOSTRING(L,"test_value_converter('converted string')");
    DOSTRING(L,"test_pointer_converter('converted string')");
#endif

    DOSTRING_EXPECTED(L, "f('incorrect', 'parameters')",
        "No matching overload found, candidates:\n"
        "int f(int,int)\n"
        "int f(int)\n"
        "Passed arguments [2]: string ('incorrect'), string ('parameters')\n");


    DOSTRING(L,"function failing_fun() error('expected error message') end");
    try
    {
        call_function<void>(L, "failing_fun");
        TEST_ERROR("function didn't fail when it was expected to");
    }
    catch(luabind::error const& e)
    {
        if (luabind::string("[string \"function failing_fun() error('expected "
            "erro...\"]:1: expected error message") != lua_tostring(L, -1))
        {
            TEST_ERROR("function failed with unexpected error message");
        }

        lua_pop(L, 1);
    }
*/
}

