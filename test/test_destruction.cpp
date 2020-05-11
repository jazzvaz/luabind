// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct cpp_class
{
    void method() {}
};

void test_main(lua_State* L)
{
    using namespace luabind;
    module(L)
    [
        class_<cpp_class>("cpp_class")
            .def(constructor<>())
            .def("method", &cpp_class::method)
    ];

    DOSTRING_EXPECTED(L,
        "t = { }\n"
        "setmetatable(t, {__gc = function(t)\n"
        "    t.cpp_class:method() end})\n"
        "t.cpp_class = cpp_class()\n"
        "t = nil\n"
        "collectgarbage()\n",
        "error in __gc metamethod ([string \"t = { }...\"]:3: attempt to index field 'cpp_class' (a userdata value))")
}
