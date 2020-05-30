// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct A
{
    const A* f() { return this; }

    int g1() const { return 1; }
    int g2() { return 2; }
};

TEST_CASE("const")
{
    using namespace luabind;

    module(L)
    [
        class_<A>("A")
            .def(constructor<>())
            .def("f", &A::f)
            .def("g", &A::g1)
            .def("g", &A::g2)
    ];

    DOSTRING(L,"a = A()");
    DOSTRING(L,"assert(a:g() == 2)");

    DOSTRING(L,"a2 = a:f()");
    DOSTRING(L,"assert(a2:g() == 1)");
}

