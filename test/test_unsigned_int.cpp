// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

TEST_CASE("unsigned int")
{
    DOSTRING(L,"x = 4294967295");

    unsigned int x = luabind::object_cast<unsigned int>(
        luabind::globals(L)["x"]);

    unsigned int y = 4294967295UL;

    CHECK(x == y);
}

