// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <memory>

struct X
{
    X(int value)
      : value(value)
    {
        ++alive;
    }

    ~X()
    {
        --alive;
    }

    int value;

    static int alive;
};

int X::alive = 0;

struct ptr
{
    ptr(X* p)
      : p(p)
    {}

    ptr(ptr const& other)
      : p(other.p)
    {
        const_cast<ptr&>(other).p = 0;
    }

	explicit operator bool() const {
		return p != nullptr;
	}

    ~ptr()
    {
		luabind::luabind_delete(p);
    }

    X* p;
};

X* get_pointer(ptr const& p)
{
    return p.p;
}

luabind::unique_ptr<X> make1()
{
	return luabind::unique_ptr<X>(luabind::luabind_new<X>(1));
}

std::shared_ptr<X> make2()
{
	return std::shared_ptr<X>(luabind::luabind_new<X>(2), luabind::luabind_delete<X>);
}

ptr make3()
{
	return ptr(luabind::luabind_new<X>(3));
}

void test_main(lua_State* L)
{
    using namespace luabind;

	static_assert(detail::has_get_pointer<luabind::unique_ptr<X>>::value,
		"missing get_pointer function for luabind::unique_ptr<X>");
	static_assert(detail::has_get_pointer<std::shared_ptr<X>>::value,
		"missing get_pointer function for std::shared_ptr<X>");
	static_assert(detail::has_get_pointer<ptr>::value,
		"missing get_pointer function for ptr");

    module(L) [
        class_<X>("X")
          .def_readonly("value", &X::value),

        def("make1", make1),
        def("make2", make2),
        def("make3", make3)
    ];

    DOSTRING(L,
        "x1 = make1()\n"
        "x2 = make2()\n"
        "x3 = make3()\n"
    );

    TEST_CHECK(X::alive == 3);

    DOSTRING(L,
        "assert(x1.value == 1)\n"
        "assert(x2.value == 2)\n"
        "assert(x3.value == 3)\n"
    );

    DOSTRING(L,
        "x1 = nil\n"
        "x2 = nil\n"
        "x3 = nil\n"
        "collectgarbage()\n"
    );

    assert(X::alive == 0);
}

