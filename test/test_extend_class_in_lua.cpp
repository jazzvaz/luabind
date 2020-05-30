// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct CppClass
{
    int f(int x)
    {
        return x;
    }
};

TEST_CASE("extend_class_in_lua")
{
    using namespace luabind;

    module(L) [
        class_<CppClass>("CppClass")
          .def(constructor<>())
          .def("f", &CppClass::f)
    ];

    DOSTRING(L,
        "function CppClass:f_in_lua(x)\n"
        "    return self:f(x) * 2\n"
        "end\n"
    );

    DOSTRING(L,
        "x = CppClass()\n"
        "assert(x:f(1) == 1)\n"
        "assert(x:f_in_lua(1) == 2)\n"
    );
}

