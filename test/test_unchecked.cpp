// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

TEST_CASE("unchecked")
{
	DOSTRING(L,
		"class 'Foo'\n"
		"function Foo:__init() end\n"
	);

	// Direct calls to metamethods used to cause access violations.
	DOSTRING_EXPECTED(L, "getmetatable(Foo).__index()",
		"[string \"getmetatable(Foo).__index()\"]:1: "
		"attempt to index a boolean value"
	);

	DOSTRING(L,"foo = Foo()");

	DOSTRING_EXPECTED(L, "getmetatable(foo).__index()",
		"[string \"getmetatable(foo).__index()\"]:1: "
		"attempt to index a boolean value"
	);
}
