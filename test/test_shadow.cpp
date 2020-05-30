// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

using namespace luabind;

struct X
{
    void f()
    {}
};

struct Y : X
{
    void f()
    {}
};

TEST_CASE("shadow")
{
    module(L) [
        class_<X>("X")
          .def(constructor<>())
          .def("f", &X::f),

        class_<Y, X>("Y")
          .def(constructor<>())
          .def("f", &Y::f)
    ];

    DOSTRING(L,
        "x = X()\n"
        "x:f()\n"
    );

    DOSTRING(L,
        "x = Y()\n"
        "x:f()\n"
    );
}

