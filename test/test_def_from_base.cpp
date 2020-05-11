// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct V
{
    int f(int,int) 
    {
        return 2; 
    }
};

struct W : V
{};

void test_main(lua_State* L)
{
    using namespace luabind;

    module(L)
    [
        class_<W>("W")
          .def(constructor<>())
          .def("f", &V::f)
    ];

    DOSTRING(L,
        "x = W()\n"
        "assert(x:f(1,2) == 2)\n"
    );
}

