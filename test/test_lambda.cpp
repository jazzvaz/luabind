// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2020 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

TEST_CASE("lambda")
{
	using namespace luabind;	
	std::function<int(int)> func1([](int i) { return i+8; });
	int loc = 99;
	auto lambda = [&](int i) { return i+loc; };
    module(L)
    [
		def("func1", func1),
		def("func2", [](int i) { return i*8; }),
		def("make_function", [&]() { return func1; }),
		def("make_lambda", [&]() { return lambda; })
    ];
	DOSTRING(L,"assert(func1(2) == 10)");
	DOSTRING(L,"assert(func2(2) == 16)");
	DOSTRING(L,
		"func = make_function()\n"
		"assert(func(2) == 10)");
	DOSTRING(L,
		"func = make_lambda()\n"
		"assert(func(1) == 100)");
}
