// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct simple_class : counted_type<simple_class>
{
    static int feedback;

    void f()
    {
        feedback = 1;
    }

    void f(int, int) {}
    void f(luabind::string a)
    {
        const char str[] = "foo\0bar";
        if (a == luabind::string(str, sizeof(str)-1))
            feedback = 2;
    }

    luabind::string g()
    {
        const char str[] = "foo\0bar";
        return luabind::string(str, sizeof(str)-1);
    }

};

int simple_class::feedback = 0;
    
COUNTER_GUARD(simple_class);

TEST_CASE("simple_class")
{
    using namespace luabind;

    using f_overload1 = void(simple_class::*)();
    using f_overload2 = void(simple_class::*)(int, int);
    using f_overload3 = void(simple_class::*)(luabind::string);

    module(L)
    [
        class_<simple_class>("simple")
            .def(constructor<>())
            .def("f", (f_overload1)&simple_class::f)
            .def("f", (f_overload2)&simple_class::f)
            .def("f", (f_overload3)&simple_class::f)
            .def("g", &simple_class::g)
    ];

    DOSTRING(L,
        "class 'simple_derived' (simple)\n"
        "  function simple_derived:__init() simple.__init(self) end\n"
        "a = simple_derived()\n"
        "a:f()\n");
    CHECK(simple_class::feedback == 1);

    DOSTRING(L,"a:f('foo\\0bar')");
    CHECK(simple_class::feedback == 2);

    DOSTRING(L,
        "b = simple_derived()\n"
        "a.foo = 'yo'\n"
        "assert(b.foo == nil)");

    DOSTRING(L,
        "simple_derived.foobar = 'yi'\n"
        "assert(b.foobar == 'yi')\n"
        "assert(a.foobar == 'yi')\n");

    simple_class::feedback = 0;

    DOSTRING_EXPECTED(L, "a:f('incorrect', 'parameters')",
        "No matching overload found, candidates:\n"
        "void f(simple&,luabind::string)\n"
        "void f(simple&,int,int)\n"
        "void f(simple&)\n"
        "Passed arguments [3]: userdata (simple&), string ('incorrect'), string ('parameters')\n");

    DOSTRING(L,"if a:g() == \"foo\\0bar\" then a:f() end");
    CHECK(simple_class::feedback == 1);
}

