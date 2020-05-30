// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2020 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>

struct base : counted_type<base>
{};

COUNTER_GUARD(base);

TEST_CASE("lua_stack")
{
    using namespace luabind;
    module(L)
    [
        class_<base>("base")
    ];
    {
        const int top0 = lua_gettop(L);
        lua_stack::push(L, 1);
        CHECK(lua_gettop(L) == top0 + 1);
        lua_stack::pop(L);
        const int top1 = lua_gettop(L);
        CHECK(top0 == top1);
    }
    {
        lua_stack::push(L, 42);
        CHECK(lua_stack::pop<int>(L) == 42);
    }
    {
        base* b = luabind_new<base>();
        lua_stack::push<policy::adopt<1>>(L, b);
        globals(L)["b"] = object(from_stack(L, -1));
        lua_stack::pop(L);
        DOSTRING(L,
            "b = nil\n"
            "collectgarbage()");
        CHECK(base::count == 0);
    }
    {
        base* b = luabind_new<base>();
        lua_stack::push<policy::adopt<1>>(L, b);
        globals(L)["b"] = object(from_stack(L, -1));
        base* nb = lua_stack::pop<base*, policy::adopt<1>>(L);
        CHECK(b == nb);
        DOSTRING(L,
            "b = nil\n"
            "collectgarbage()");
        CHECK(base::count == 1);
        luabind_delete(b);
    }
}
