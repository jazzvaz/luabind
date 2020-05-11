// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <iostream>
#include <luabind/luabind.hpp>

void greet()
{
    std::cout << "hello world!\n";
}

extern "C" int init(lua_State* L)
{
    using namespace luabind;

    open(L);

    module(L)
    [
        def("greet", &greet)
    ];

    return 0;
}

