// Copyright Daniel Wallin 2009. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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

void test_main(lua_State* L)
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
	TEST_CHECK(spx.use_count() == 2);
	DOSTRING(L,
		"sx = nil\n"
		"collectgarbage()\n"
	);
	TEST_CHECK(spx.use_count() == 1);
	TEST_CHECK(X::alive == 1);
	spx.reset(); // reference to lua object is released here
	DOSTRING(L,
		"collectgarbage()\n" // unreferenced object is collected
	);
	TEST_CHECK(X::alive == 0);
}

