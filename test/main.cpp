// Copyright (c) 2005 Daniel Wallin, Arvid Norberg

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

#include <luabind/lua_include.hpp>

#ifndef LUABIND_CPLUSPLUS_LUA
extern "C"
{
#endif
# include <lualib.h>
#ifndef LUABIND_CPLUSPLUS_LUA
}
#endif

#include <luabind/open.hpp>             // for open

#include <cstring>                      // for strlen
#include <exception>                    // for exception
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

void test_main(lua_State*);

struct lua_state
{
	lua_state();
	~lua_state();

	operator lua_State*() const;
	void check() const;

private:
	lua_State* m_state;
	int m_top;
};

#ifdef LUABIND_CUSTOM_ALLOCATOR
#include <algorithm>
#include <iterator>

struct la_tag
{
	static const char ref_data[16];
	char data[16];

	void init()
	{
		std::copy(std::begin(ref_data), std::end(ref_data), std::begin(data));
	}

	bool verify()
	{
		return std::equal(std::begin(ref_data), std::end(ref_data), std::begin(data));
	}
};

const char la_tag::ref_data[16] = " [luabind_mem] ";

static void* __cdecl luabind_allocator(void* context, const void* pointer, size_t const size)
{
	if (!size)
	{
		void* nc_ptr = const_cast<void*>(pointer);
		la_tag* tag = reinterpret_cast<la_tag*>(reinterpret_cast<char*>(nc_ptr) - sizeof(la_tag));
		if (!tag->verify())
			TEST_ERROR("luabind::allocator: trying to deallocate unrecognized memory block");
		free(tag);
		return nullptr;
	}
	if (!pointer)
	{
		la_tag* tag = reinterpret_cast<la_tag*>(malloc(size + sizeof(la_tag)));
		tag->init();
		return reinterpret_cast<char*>(tag) + sizeof(la_tag);
	}
	void* nc_ptr = const_cast<void*>(pointer);
	la_tag* tag = reinterpret_cast<la_tag*>(reinterpret_cast<char*>(nc_ptr) - sizeof(la_tag));
	if (!tag->verify())
		TEST_ERROR("luabind::allocator: trying to reallocate unrecognized memory block");
	return realloc(tag, size + sizeof(la_tag));
}
#endif

lua_state::lua_state()
	: m_state(luaL_newstate())
{
#ifdef LUABIND_CUSTOM_ALLOCATOR
	luabind::allocator = &luabind_allocator;
	luabind::allocator_context = nullptr;
#endif
	luaopen_base(m_state);
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
	// lua 5.1 or newer
	luaL_openlibs(m_state);
#else
	// lua 5.0.2 or older
	lua_baselibopen(m_state);
#endif
	m_top = lua_gettop(m_state);
	luabind::open(m_state);
}

lua_state::~lua_state()
{
	lua_close(m_state);
}

void lua_state::check() const
{
	TEST_CHECK(lua_gettop(m_state) == m_top);
}

lua_state::operator lua_State*() const
{
	return m_state;
}

int pcall_handler(lua_State* /*L*/)
{
	return 1;
}

void dostring(lua_State* state, char const* str)
{
	lua_pushcclosure(state, &pcall_handler, 0);

	if(luaL_loadbuffer(state, str, std::strlen(str), str))
	{
		luabind::string err(lua_tostring(state, -1));
		lua_pop(state, 2);
		throw err;
	}

	if(lua_pcall(state, 0, 0, -2))
	{
		luabind::string err(lua_tostring(state, -1));
		lua_pop(state, 2);
		throw err;
	}

	lua_pop(state, 1);
}

bool tests_failure = false;

void report_failure(char const* err, char const* file, int line)
{
	std::cout << file << ":" << line << "\"" << err << "\"\n";
	tests_failure = true;
}

int main()
{
	lua_state L;
#ifndef LUABIND_NO_EXCEPTIONS
	try
#endif
	{
		test_main(L);
		L.check();
		return tests_failure ? 1 : 0;
	}
#ifndef LUABIND_NO_EXCEPTIONS
	catch(std::exception const& e)
	{
		std::cerr << "Terminated with exception: \"" << e.what() << "\"\n";
		return 1;
	}
	catch(...)
	{
		std::cerr << "Terminated with unknown exception\n";
		return 1;
	}
#endif
}

