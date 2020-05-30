// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

void f(luabind::table<> const& x)
{
    CHECK(luabind::type(x) == LUA_TTABLE);
}

void g(luabind::table<luabind::argument> const& x)
{
    CHECK(luabind::type(x) == LUA_TTABLE);
}

TEST_CASE("table")
{
    using namespace luabind;

    module(L) [
        def("f", &f),
        def("g", &g)
    ];

    DOSTRING(L,
        "f {1,2,3}\n"
        "g {1,2,3}\n"
    );

    DOSTRING_EXPECTED(L,
        "f(1)\n",
        "No matching overload found, candidates:\n"
        "void f(table const&)\n"
        "Passed arguments [1]: number (1)\n"
    );

    DOSTRING_EXPECTED(L,
        "g(1)\n",
        "No matching overload found, candidates:\n"
        "void g(table const&)\n"
        "Passed arguments [1]: number (1)\n"
    );
}

