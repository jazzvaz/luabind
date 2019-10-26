// Copyright (c) 2003 Daniel Wallin, Arvid Norberg, and contributors

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

void test_main(lua_State* L)
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
	DOSTRING(L, "x = get()\n");
	TEST_CHECK(instance.val1 == 1);
	TEST_CHECK(instance.val2 == 2);
	TEST_CHECK(instance.prop == 0);

	DOSTRING(L, "assert(x.a == 1)\n");
	DOSTRING(L, "assert(x.b == 2)\n");

	DOSTRING(L, "x.a = 5\n");
	TEST_CHECK(instance.val1 == 5);
	TEST_CHECK(instance.val2 == 2);
	TEST_CHECK(instance.prop == 0);

	DOSTRING(L, "x.b = 10\n");
	TEST_CHECK(instance.val1 == 5);
	TEST_CHECK(instance.val2 == 10);
	TEST_CHECK(instance.prop == 0);

	// Check if properties work
	DOSTRING(L, "x.prop = 20\n");
	TEST_CHECK(instance.val1 == 5);
	TEST_CHECK(instance.val2 == 10);
	TEST_CHECK(instance.prop == 20);
}
