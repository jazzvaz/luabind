// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

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

TEST_CASE("adopt_wrapper")
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
