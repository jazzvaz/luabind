// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/shared_ptr_converter.hpp>

struct X
{
    X(int value)
      : value(value)
    {
        alive++;
    }

    ~X()
    {
        alive--;
    }

    int value;
    static int alive;
};

int X::alive = 0;

int get_value(std::shared_ptr<X> const& p)
{
    return p->value;
}

std::shared_ptr<X> filter(std::shared_ptr<X> const& p)
{
    return p;
}

std::shared_ptr<X> make_x()
{
    return std::shared_ptr<X>(luabind::luabind_new<X>(0), luabind::luabind_delete<X>);
}

TEST_CASE("shared_ptr")
{
    using namespace luabind;

    module(L) [
        class_<X>("X")
            .def(constructor<int>()),
        def("get_value", &get_value),
        def("filter", &filter),
        def("make_x", &make_x)
    ];

    DOSTRING(L,
        "x = X(1)\n"
        "assert(get_value(x) == 1)\n"
    );

    DOSTRING(L,
        "assert(x == filter(x))\n"
        "x = nil\n"
        "collectgarbage()\n"
    );
    DOSTRING(L,
        "sx = make_x()\n"
        "function get_sx() return sx end"
    );
    std::shared_ptr<X> spx = call_function<std::shared_ptr<X>>(L, "get_sx");
    CHECK(spx.use_count() == 2);
    DOSTRING(L,
        "sx = nil\n"
        "collectgarbage()\n"
    );
    CHECK(spx.use_count() == 1);
    CHECK(X::alive == 1);
    spx.reset(); // reference to lua object is released here
    DOSTRING(L,
        "collectgarbage()\n" // unreferenced object is collected
    );
    CHECK(X::alive == 0);
}

