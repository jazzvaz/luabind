// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2015 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>

static void f() {}

TEST_CASE("function_overload_overflow")
{
	using namespace luabind;
	module(L)
	[ // 11 functions
		def("f", &f), // 1
		def("f", &f), // 2
		def("f", &f), // 3
		def("f", &f), // 4
		def("f", &f), // 5
		def("f", &f), // 6
		def("f", &f), // 7
		def("f", &f), // 8
		def("f", &f), // 9
		def("f", &f), // 10
		def("f", &f)  // 11
	];
	DOSTRING_EXPECTED(L, "f()",
		"Ambiguous, candidates:\n" // 11 candidates
		"void f()\n" // 1
		"void f()\n" // 2
		"void f()\n" // 3
		"void f()\n" // 4
		"void f()\n" // 5
		"void f()\n" // 6
		"void f()\n" // 7
		"void f()\n" // 8
		"void f()\n" // 9
		"void f()\n" // 10
		"and 1 additional overload(s) not shown\n" // 11
		"Passed arguments [0]: <no arguments>\n");
}
