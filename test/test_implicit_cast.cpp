// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <memory>

struct A : counted_type<A>
{
	virtual ~A() {}
};

struct B : A, counted_type<B>
{};


struct enum_placeholder {};
enum LBENUM_t { VAL1 = 1, VAL2 = 2 };
LBENUM_t enum_by_val(LBENUM_t e) { return e; }
LBENUM_t enum_by_const_ref(const LBENUM_t &e) { return e; }



struct test_implicit : counted_type<test_implicit>
{
	char const* f(A*) { return "f(A*)"; }
	char const* f(B*) { return "f(B*)"; }
};

struct char_pointer_convertable
	: counted_type<char_pointer_convertable>
{
	operator const char*() const { return "foo!"; }
};

void func(const char_pointer_convertable& /*f*/)
{
}

void convert(std::shared_ptr<A>)
{
}

int f(int a)
{
	return a;
}

COUNTER_GUARD(A);
COUNTER_GUARD(B);
COUNTER_GUARD(test_implicit);
COUNTER_GUARD(char_pointer_convertable);

TEST_CASE("implicit_cast")
{
	using namespace luabind;

	using f1 = char const*(test_implicit::*)(A*);
	using f2 = char const*(test_implicit::*)(B*);

	module(L)
		[
			class_<A>("A")
			.def(constructor<>()),

		class_<B, A>("B")
		.def(constructor<>()),

		class_<test_implicit>("test")
		.def(constructor<>())
		.def("f", (f1)&test_implicit::f)
		.def("f", (f2)&test_implicit::f),

		class_<char_pointer_convertable>("char_ptr")
		.def(constructor<>()),

		class_<enum_placeholder>("LBENUM")
		.enum_("constants")
		[
			value("VAL1", VAL1),
			value("VAL2", VAL2)
		],
		def("enum_by_val", &enum_by_val),
		def("enum_by_const_ref", &enum_by_const_ref),

		def("func", &func),
		def("convert", &convert),
		def("f", &f)
		];

	DOSTRING(L,"a = A()");
	DOSTRING(L,"b = B()");
	DOSTRING(L,"t = test()");

	DOSTRING(L,"assert(t:f(a) == 'f(A*)')");
	DOSTRING(L,"assert(t:f(b) == 'f(B*)')");

	DOSTRING(L,
		"a = char_ptr()\n"
		"func(a)");

	DOSTRING(L,"assert(LBENUM.VAL1 == 1)");
	DOSTRING(L,"assert(LBENUM.VAL2 == 2)");
	DOSTRING(L,"assert(enum_by_val(LBENUM.VAL1) == LBENUM.VAL1)");
	DOSTRING(L,"assert(enum_by_val(LBENUM.VAL2) == LBENUM.VAL2)");
	DOSTRING(L,"assert(enum_by_const_ref(LBENUM.VAL1) == LBENUM.VAL1)");
	DOSTRING(L,"assert(enum_by_const_ref(LBENUM.VAL2) == LBENUM.VAL2)");

	DOSTRING(L,
		"a = A()\n"
		"convert(a)");

	DOSTRING_EXPECTED(L,
		"a = nil\n"
		"f(a)",
		"No matching overload found, candidates:\n"
		"int f(int)\n"
		"Passed arguments [1]: nil (nil)\n");
}

