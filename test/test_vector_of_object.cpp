// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#include "test.hpp"
#include <luabind/object.hpp>
#include <luabind/detail/conversion_policies/conversion_policies.hpp>

using namespace luabind;

void test_main(lua_State* L)
{
    std::vector<object> v;
    v.push_back(object(L, 0));

    for (std::vector<object>::iterator i(v.begin()), e(v.end()); i != e; ++i)
    {}
}

