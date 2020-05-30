// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/copy_policy.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/discard_result_policy.hpp>
#include <luabind/dependency_policy.hpp>

struct test_copy {};
struct secret_type {};

secret_type sec_;

struct policies_test_class
{
    policies_test_class(const char* name): name_(name)
    { ++count; }
    policies_test_class() { ++count; }
    ~policies_test_class()
    { --count; }

    luabind::string name_;

    policies_test_class* make(const char* name) const
    {
        return new policies_test_class(name);
    }

    void f(policies_test_class* p)
    {
        delete p;
    }
    
    const policies_test_class* internal_ref() {
        return this;
    }
    
    policies_test_class* self_ref()
    { 
        return this;
    }

    static int count;

    //    private:
    policies_test_class(policies_test_class const& c)
        : name_(c.name_)
    { 
        ++count;
    }

    void member_pure_out_val(int a, int* v) {
        *v = a * 2;
    }
    
    void member_out_val(int a, int* v) { 
        *v *= a;
    }

    secret_type* member_secret() { return &sec_; }
};

int policies_test_class::count = 0;

policies_test_class global;

void out_val(float* f) { *f = 3.f; }
policies_test_class* copy_val() { return &global; }
policies_test_class const* copy_val_const() { return &global; }

secret_type* secret() { return &sec_; }

void aux_test();

struct test_t {
    test_t *make(int) { return new test_t(); }
    void take(test_t*) {}
};

struct MI2;

struct MI1
{
    void add(MI2 *) {}
};

struct MI2 : public MI1
{
    virtual ~MI2()
    {}
};

struct MI2W : public MI2, public luabind::wrap_base {};


void function_test1(std::function<void(int, int)> func) {
    func(3, 4);
}


int function_test2_impl(int a, int b)
{
    return a+b;
}

std::function<int(int, int)> function_test2()
{
    return std::function<int(int, int)>(function_test2_impl);
}

TEST_CASE("policies")
{
    using namespace luabind;

    module(L)
    [
        class_<test_t>("test_t")
        .def("make", &test_t::make, policy::adopt<0>())
        .def("take", &test_t::take, policy::adopt<2>())
    ];
    
    module(L)
    [
        class_<policies_test_class>("test")
            .def(constructor<>())
            .def("member_pure_out_val", &policies_test_class::member_pure_out_val, policy::pure_out_value<3>())
            .def("member_out_val", &policies_test_class::member_out_val, policy::out_value<3>())
            .def("member_secret", &policies_test_class::member_secret, policy::discard_result())
            .def("f", &policies_test_class::f, policy::adopt<2>())
            .def("make", &policies_test_class::make, policy::adopt<0>())
            .def("internal_ref", &policies_test_class::internal_ref, policy::dependency<0,1>())
            .def("self_ref", &policies_test_class::self_ref, policy::return_reference_to<1>()),

        def("out_val", &out_val, policy::pure_out_value<1>()),
        def("copy_val", &copy_val, policy::copy<0>()),
        def("copy_val_const", &copy_val_const, policy::copy<0>()),
        def("secret", &secret, policy::discard_result()),

        def("function_test1", &function_test1),
        def("function_test2", &function_test2),

        class_<MI1>("mi1")
            .def(constructor<>())
            .def("add",&MI1::add, policy::adopt<2>()),

        class_<MI2,MI1,default_holder,MI2W>("mi2")
            .def(constructor<>())
    ];

    // test copy
    DOSTRING(L,"a = secret()\n");

    CHECK(policies_test_class::count == 1);

    DOSTRING(L,"a = copy_val()\n");
    CHECK(policies_test_class::count == 2);

    DOSTRING(L,"b = copy_val_const()\n");
    CHECK(policies_test_class::count == 3);

    DOSTRING(L,
        "a = nil\n"
        "b = nil\n"
        "collectgarbage()\n");

    // only the global variable left here
    CHECK(policies_test_class::count == 1);

    // out_value
    DOSTRING(L,
        "a = out_val()\n"
        "assert(a == 3)");

    // return_reference_to
    DOSTRING(L,
        "a = test()\n"
        "b = a:self_ref()\n"
        "a = nil\n"
        "collectgarbage()");

    // This one goes wrong
    // a is kept alive as long as b is alive
    CHECK(policies_test_class::count == 2);

    DOSTRING(L,
        "b = nil\n"
        "collectgarbage()");

    CHECK(policies_test_class::count == 1);

    DOSTRING(L,"a = test()");

    CHECK(policies_test_class::count == 2);

    DOSTRING(L,
        "b = a:internal_ref()\n"
        "a = nil\n"
        "collectgarbage()");

    // a is kept alive as long as b is alive
    CHECK(policies_test_class::count == 2);

    // two gc-cycles because dependency-table won't be collected in the
    // same cycle as the object_rep
    DOSTRING(L,
        "b = nil\n"
        "collectgarbage()\n"
        "collectgarbage()");

    CHECK(policies_test_class::count == 1);

    // adopt
    DOSTRING(L,"a = test()");

    CHECK(policies_test_class::count == 2);

    DOSTRING(L,"b = a:make('tjosan')");
    DOSTRING(L,"assert(a:member_pure_out_val(3) == 6)");
    DOSTRING(L,"assert(a:member_out_val(3,2) == 6)");
    DOSTRING(L,"a:member_secret()");

    // make instantiated a new policies_test_class
    CHECK(policies_test_class::count == 3);

    DOSTRING(L,"a:f(b)\n");

    // b was adopted by c++ and deleted the object
    CHECK(policies_test_class::count == 2);

    DOSTRING(L,"a = nil\n"
        "collectgarbage()");

    CHECK(policies_test_class::count == 1);

    // adopt with wrappers
    DOSTRING(L,"mi1():add(mi2())");

    // function converter
    DOSTRING(L,
        "result = nil\n"
        "test = function( a, b ) result = a + b; end\n"
        "function_test1( test )\n"
        "assert(result == 7)\n"
        );

    DOSTRING(L,
             "local func = function_test2()\n"
             "assert(func(4,5)==9)"
        );
}

