// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/version.hpp>
#include <memory>

struct base : counted_type<base>
{
    base(): n(4) {}
    virtual ~base() {}

    void f(int)
    {
    }

    int n;
};
    
// this is here to make sure the pointer offsetting works
struct first_base : counted_type<first_base>
{
    virtual ~first_base() {}
    virtual void a() {}
    int padding;
};

struct derived : first_base, base
{
    derived(): n2(7) {}
    void f() {}
    int n2;
};

COUNTER_GUARD(first_base);
COUNTER_GUARD(base);

int feedback = 0;

void tester(base* t)
{
    if (t->n == 4) feedback = 1;
}

void tester_(derived* t)
{
    if (t->n2 == 7) feedback = 2;
}

void tester2(std::shared_ptr<base> t)
{
    if (t->n == 4) feedback = 3;
}

void tester3(std::shared_ptr<const base> t)
{
    if (t->n == 4) feedback = 4;
}

void tester4(const std::shared_ptr<const base>& t)
{
    if (t->n == 4) feedback = 5;
}

void tester5(const std::shared_ptr<const base>* t)
{
    if ((*t)->n == 4) feedback = 6;
}

void tester6(const std::shared_ptr<base>* t)
{
    if ((*t)->n == 4) feedback = 7;
}

void tester7(std::shared_ptr<base>* t)
{
    if ((*t)->n == 4) feedback = 8;
}

std::shared_ptr<base> tester9()
{
    feedback = 9;
    return std::shared_ptr<base>(new base());
}

void tester10(std::shared_ptr<base> const& r)
{
    if (r->n == 4) feedback = 10;
}

void tester11(std::shared_ptr<const base> const& r)
{
    if (r->n == 4) feedback = 11;
}

void tester12(std::shared_ptr<derived> const& r)
{
    if (r->n2 == 7) feedback = 12;
}

derived tester13()
{
    feedback = 13;
    derived d;
    d.n2 = 13;
    return d;
}

TEST_CASE("held_type")
{
    std::shared_ptr<base> base_ptr(new base());

    using namespace luabind;
  
    module(L)
    [
        def("tester", &tester),
        def("tester", &tester_),
        def("tester2", &tester2),
        def("tester3", &tester3),
        def("tester4", &tester4),
        def("tester5", &tester5),
        def("tester6", &tester6),
        def("tester7", &tester7),
        def("tester9", &tester9),
        def("tester10", &tester10),
        def("tester11", &tester11),
        def("tester12", &tester12),
        def("tester13", &tester13),

        class_<base, no_bases, std::shared_ptr<base> >("base")
            .def(constructor<>())
            .def("f", &base::f),

        class_<derived, base, std::shared_ptr<base> >("derived")
            .def(constructor<>())
            .def("f", &derived::f)
    ];

    object g = globals(L);
    g["ptr"] = base_ptr;

    DOSTRING(L,"tester(ptr)");
    CHECK(feedback == 1);

    DOSTRING(L,
        "a = base()\n"
        "b = derived()\n");

#if LUABIND_VERSION != 900
    DOSTRING(L,"tester(b)");
    CHECK(feedback == 2);
#endif

    DOSTRING(L,"tester(a)");
    CHECK(feedback == 1);

    DOSTRING(L,"tester2(b)");
    CHECK(feedback == 3);

    feedback = 0;

    DOSTRING(L,"tester10(b)");
    CHECK(feedback == 10);

/* this test is messed up, shared_ptr<derived> isn't even registered
    DOSTRING_EXPECTED(L,
        L
        , "tester12(b)"
        , "no match for function call 'tester12' with the parameters (derived)\n"
        "candidates are:\n"
        "tester12(const custom&)\n");
*/
#if LUABIND_VERSION != 900
    object nil = globals(L)["non_existing_variable_is_nil"];
    CHECK(object_cast<boost::shared_ptr<base> >(nil).get() == 0);
    CHECK(object_cast<boost::shared_ptr<const base> >(nil).get() == 0);
#endif

    DOSTRING(L,"tester13()");
    CHECK(feedback == 13);
}

