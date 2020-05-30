// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <luabind/error.hpp>
#include <luabind/lua_include.hpp>
#include <string>
#include <doctest/doctest.h>

#define DOSTRING_EXPECTED(state, str, expected) \
    CHECK_THROWS_WITH((dostring(state, str)), expected)
#define DOSTRING(state, str) dostring(state, str)

struct lua_state
{
	~lua_state();
	void initialize();
	operator lua_State*() const { return m_state; }
	void check() const;

private:
	lua_State* m_state = nullptr;
	int m_top = 0;
};

inline lua_State *L = nullptr;

std::string demangle(std::type_info const& id);

#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)

#ifdef _MSC_VER
// There seem to be lifetime issues with global variables on VC
#define COUNTER_GUARD(type)
#else
#define COUNTER_GUARD(type) \
    struct CAT(type, _counter_guard) \
    { \
        ~CAT(type, _counter_guard()) \
        { \
            CHECK_MESSAGE(counted_type<type>::count == 0, \
                demangle(typeid(type)) << ": live object"); \
        } \
    } CAT(type, _guard)
#endif

//#undef CAT
//#undef CAT2

//#define TEST_NOTHROW(x) \
//	try \
//	{ \
//		x; \
//	} \
//	catch (...) \
//	{ \
//		TEST_ERROR("Exception thrown: " #x); \
//	}

void dostring(lua_State* L, char const* str);

template <class T>
struct counted_type
{
    inline static int count = 0;
    
    counted_type() { ++count; }
    counted_type(counted_type const&) { ++count; }
    ~counted_type()
    { CHECK_MESSAGE(--count >= 0, demangle(typeid(T)) << ": double destruction"); }
};

//inline void DOSTRING_EXPECTED_X(char const* str, char const* expected)
//{
//    try
//    {
//        dostring(L, str);
//    }
//    catch (luabind::error const& e)
//    {
//		using namespace std;
//		if (std::strcmp(e.what(), expected))
//            TEST_ERROR(e.what());
//    }
//    catch (luabind::string const& s)
//    {
//        if (s != expected)
//            TEST_ERROR(s.c_str());
//    }
//}
