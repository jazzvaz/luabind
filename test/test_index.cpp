// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <type_traits>

class indexable
{
public:
    int val1 = 1;
    int val2 = 2;
    int prop = 0;

    int index(const char* key)
    {
        if (*key == 'a')
        {
            return val1;
        }
        return val2;
    }

    void newindex(const char* key, int v)
    {
        if (*key == 'a')
        {
            val1 = v;
            return;
        }
        val2 = v;
    }
};

static indexable instance;

indexable* get()
{
    return &instance;
}

TEST_CASE("index")
{
    using namespace luabind;

    module(L)
    [
        class_<indexable>("test")
            .index(&indexable::index)
            .newindex(&indexable::newindex)
            .def_readwrite("prop", &indexable::prop),

        def("get", &get)
    ];
    DOSTRING(L,"x = get()\n");
    CHECK(instance.val1 == 1);
    CHECK(instance.val2 == 2);
    CHECK(instance.prop == 0);

    DOSTRING(L,"assert(x.a == 1)\n");
    DOSTRING(L,"assert(x.b == 2)\n");

    DOSTRING(L,"x.a = 5\n");
    CHECK(instance.val1 == 5);
    CHECK(instance.val2 == 2);
    CHECK(instance.prop == 0);

    DOSTRING(L,"x.b = 10\n");
    CHECK(instance.val1 == 5);
    CHECK(instance.val2 == 10);
    CHECK(instance.prop == 0);

    // Check if properties work
    DOSTRING(L,"x.prop = 20\n");
    CHECK(instance.val1 == 5);
    CHECK(instance.val2 == 10);
    CHECK(instance.prop == 20);
}
