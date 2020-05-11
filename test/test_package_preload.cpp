// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/set_package_preload.hpp>
#include <cstring>

int f(int x)
{
    return x + 1;
}

static void loader(lua_State* L, char const* modname)
{
	TEST_CHECK(modname);
	TEST_CHECK(modname && !std::strcmp(modname, "testmod"));
    using namespace luabind;
    module(L)
    [
        def("f", &f)
    ];
}

static luabind::object local_loader(lua_State* L, char const* modname)
{
	TEST_CHECK(modname);
	TEST_CHECK(modname && !std::strcmp(modname, "testmod_l"));
	using namespace luabind;
	object modtable = newtable(L);
	module(modtable)
	[
		def("f", &f)
	];
	return modtable;
}

void test_main(lua_State* L)
{
    using namespace luabind;
    
    set_package_preload(L, "testmod", &loader);
	DOSTRING(L, "assert(require('testmod') == true)");
    
    DOSTRING(L, "assert(f(7) == 8)");

	set_package_preload(L, "testmod_l", &local_loader);
	DOSTRING(L, "mod = require('testmod_l')");
	DOSTRING(L,
		"assert(not testmod_l)\n" // No global should be created.
		"assert(mod.f(41) == 42)\n"); // Module should be returned.

	DOSTRING(L, "package.preload = nil");
	try
	{
		set_package_preload(L, "failmod", &loader);
	}
	catch (luabind::error const& e)
	{
		TEST_CHECK(!std::strcmp("attempt to index a nil value", e.what()));
		return;
	}
	TEST_ERROR("set_package_preload() should have thrown.");
}

