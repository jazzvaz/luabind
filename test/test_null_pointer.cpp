// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct A : counted_type<A>
{
	A* f() { return 0; }
};

A* return_pointer()
{
	return 0;
}

COUNTER_GUARD(A);

TEST_CASE("null_pointer")
{
	using namespace luabind;

	module(L)
	[
		class_<A>("A")
			.def(constructor<>())
			.def("f", &A::f),

		def("return_pointer", &return_pointer)
	];


	DOSTRING(L,
		"e = return_pointer()\n"
		"assert(e == nil)");

	DOSTRING(L,
		"a = A()\n"
		"e = a:f()\n"
		"assert(e == nil)");
}

