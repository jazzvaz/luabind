// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

using namespace luabind;

struct base0
{
    virtual ~base0() {}
};

struct base_wrap0 : base0, wrap_base
{};

struct base1
{
    virtual ~base1() {}
};

struct base_wrap1 : base1, wrap_base
{};

base0* filter0(base0* p)
{
    return p;
}

std::shared_ptr<base1> filter1(std::shared_ptr<base1> const& p)
{
    return p;
}

TEST_CASE("back_reference")
{
    module(L)
    [
        class_<base0, no_bases, default_holder, base_wrap0>("base0")
          .def(constructor<>()),
        def("filter0", &filter0),

		class_<base1, no_bases, std::shared_ptr<base1>, base_wrap1 >("base1")
          .def(constructor<>()),
        def("filter1", &filter1)
    ];

    DOSTRING(L,
        "class 'derived0' (base0)\n"
        "  function derived0:__init()\n"
        "    base0.__init(self)\n"
        "  end\n"

        "class 'derived1' (base1)\n"
        "  function derived1:__init()\n"
        "    base1.__init(self)\n"
        "  end\n"
    );

    DOSTRING(L,
        "x = derived0()\n"
        "y = filter0(x)\n"
        "assert(x == y)\n"
    );

    DOSTRING(L,
        "x = derived1()\n"
        "y = filter1(x)\n"
        "assert(x == y)\n"
    );
}

