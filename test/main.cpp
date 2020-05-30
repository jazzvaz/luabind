// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#define DOCTEST_CONFIG_IMPLEMENT
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

#include <luabind/open.hpp> // for open
#include <cstring> // for strlen
#include <exception> // for exception
#include <iostream> // for operator<<, basic_ostream, etc
#include <string> // for string

#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
# define LUABIND_TEST_DEMANGLE_TYPENAMES
# include <cxxabi.h>
#endif

std::string demangle(std::type_info const& id)
{
#ifdef LUABIND_TEST_DEMANGLE_TYPENAMES
	int status;
	char* buf = abi::__cxa_demangle(id.name(), 0, 0, &status);
	if (buf)
	{
		std::string name(buf);
		std::free(buf);
		return name;
	}
#endif
	return id.name();
}

#ifdef LUABIND_CUSTOM_ALLOCATOR
#include <algorithm>
#include <iterator>

struct la_tag
{
	inline static const char ref_data[16] = " [luabind_mem] ";
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

static void* luabind_allocator(void* context, const void* pointer, size_t const size)
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

void lua_state::initialize()
{
	if (m_state)
		return;
	m_state = luaL_newstate();
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

lua_state::~lua_state() { lua_close(m_state); }

void lua_state::check() const
{ REQUIRE(lua_gettop(m_state) == m_top); }

static int pcall_handler(lua_State*)
{ return 1; }

void dostring(lua_State* L, char const* str)
{
	lua_pushcclosure(L, &pcall_handler, 0);
	if (luaL_loadbuffer(L, str, std::strlen(str), str))
	{
		std::string err(lua_tostring(L, -1));
		lua_pop(L, 2);
		throw std::runtime_error(err);
	}
	if (lua_pcall(L, 0, 0, -2))
	{
		std::string err(lua_tostring(L, -1));
		lua_pop(L, 2);
		throw std::runtime_error(err);
	}
	lua_pop(L, 1);
}

int main(int argc, char** argv)
{
	doctest::Context context;
	context.setOption("abort-after", 5);
	context.setOption("order-by", "name");
	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true);
	context.setAsDefaultForAssertsOutOfTestCases();
	int ret = 0;
	{
		lua_state state;
		state.initialize();
		L = state;
		ret = context.run();
		state.check();
	}
	return ret;
}
