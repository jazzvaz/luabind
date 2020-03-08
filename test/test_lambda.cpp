// Copyright (c) 2004 Daniel Wallin and contributors

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include "test.hpp"
#include <luabind/luabind.hpp>

void test_main(lua_State* L)
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
	DOSTRING(L, "assert(func1(2) == 10)");
	DOSTRING(L, "assert(func2(2) == 16)");
	DOSTRING(L,
		"func = make_function()\n"
		"assert(func(2) == 10)");
	DOSTRING(L,
		"func = make_lambda()\n"
		"assert(func(1) == 100)");
}
