// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct X
{
private:
    ~X() {}
};

int ptr_count = 0;

template <class T>
struct ptr
{
    ptr()
      : p(0)
    {
        ptr_count++;
    }

    ptr(T* p)
      : p(p)
    {
        ptr_count++;
    }

    ptr(ptr const& other)
      : p(other.p)
    {
        ptr_count++;
    }

    explicit operator bool() const {
        return p != nullptr;
    }

    template <class U>
    ptr(ptr<U> const& other)
      : p(other.p)
    {
        ptr_count++;
    }

    ~ptr()
    {
        ptr_count--;
    }

    T* p;
};

template <class T>
T* get_pointer(ptr<T> const& x)
{
    return x.p;
}

template <class T>
ptr<T const>* get_const_holder(ptr<T>*)
{
    return 0;
}

void f1(X const&)
{}

void f2(X&)
{}

void f3(X const*)
{}

void f4(X*)
{}

void g1(ptr<X> p)
{
    CHECK(ptr_count == (p.p ? 2 : 3));
}

void g2(ptr<X> const& p)
{
    CHECK(ptr_count == (p.p ? 1 : 2));
}

void g3(ptr<X>*)
{
    CHECK(ptr_count == 1);
}

void g4(ptr<X> const*)
{
    CHECK(ptr_count == 1);
}

ptr<X> get()
{
    return ptr<X>(luabind::luabind_new<X>());
}

TEST_CASE("private_destructors")
{
    using namespace luabind;

    module(L) [
        class_<X, no_bases, ptr<X> >("X"),
        def("get", &get),

        def("f1", &f1),
        def("f2", &f2),
        def("f3", &f3),
        def("f4", &f4),

        def("g1", &g1),
        def("g2", &g2),
        def("g3", &g3),
        def("g4", &g4)
    ];

    DOSTRING(L,"x = get()\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"f1(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"f2(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"f3(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"f4(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"g1(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"g2(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"g3(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,"g4(x)\n");
    CHECK(ptr_count == 1);

    DOSTRING(L,
        "x = nil\n"
    );

    lua_gc(L, LUA_GCCOLLECT, 0);
    CHECK(ptr_count == 0);
}

