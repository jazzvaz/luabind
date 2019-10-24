// Copyright Daniel Wallin 2008. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>

struct X
{
    virtual ~X()
    {}
};

struct X_wrap : X, luabind::wrap_base
{};

X* make()
{
    return luabind::luabind_new<X>();
}

void take(X* p)
{
	luabind::luabind_delete(p);
}

void test_main(lua_State* L)
{
    using namespace luabind;

    module(L) [
        class_<X, no_bases, X_wrap>("X"),
        def("make", &make, policy::adopt<0>()),
        def("take", &take, policy::adopt<1>())
    ];

    DOSTRING(L,
        "take(make())\n"
    );
}
