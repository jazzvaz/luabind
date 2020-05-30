// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

int count = 0;

struct X
{
    X()
    {
        ++count;
    }

    ~X()
    {
        --count;
    }
};

struct Y
{
    virtual ~Y()
    {}
};

struct Y_wrap : Y, luabind::wrap_base
{};

TEST_CASE("create_in_thread")
{
    using namespace luabind;

    module(L) [
        class_<X>("X")
            .def(constructor<>()),

        class_<Y, no_bases, default_holder, Y_wrap>("Y")
            .def(constructor<>())
    ];

    DOSTRING(L,
        "class 'Y_lua' (Y)\n"
        "  function Y_lua.__init(self)\n"
        "      Y.__init(self)\n"
        "  end\n"
    );

    for (int i = 0; i < 100; ++i)
    {
        lua_State* thread = lua_newthread(L);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);

        DOSTRING(thread,
            "local x = X()\n"
        );

        DOSTRING(thread,
            "local y = Y_lua()\n"
        );

        luaL_unref(L, LUA_REGISTRYINDEX, ref);
    }
}

