// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#ifndef TEST_050415_HPP
#define TEST_050415_HPP

#include <luabind/error.hpp>
#include <luabind/lua_include.hpp>
#include <string>

void report_failure(char const* str, char const* file, int line);

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
            TEST_CHECK(counted_type<type>::count == 0); \
        } \
    } CAT(type, _guard)
#endif

#define TEST_REPORT_AUX(x, line, file) \
	report_failure(x, line, file)

#define TEST_CHECK(x) \
    if (!(x)) \
        TEST_REPORT_AUX("TEST_CHECK failed: \"" #x "\"", __FILE__, __LINE__)

#define TEST_ERROR(x) \
	TEST_REPORT_AUX((luabind::string("ERROR: \"") + x + "\"").c_str(), __FILE__, __LINE__)

#define TEST_NOTHROW(x) \
	try \
	{ \
		x; \
	} \
	catch (...) \
	{ \
		TEST_ERROR("Exception thrown: " #x); \
	}

void dostring(lua_State* L, char const* str);

template<class T>
struct counted_type
{
    static int count;
    
    counted_type()
    {
        ++count;
    }

    counted_type(counted_type const&)
    {
        ++count;
    }

    ~counted_type()
    {
        TEST_CHECK(--count >= 0);
    }
};

template<class T>
int counted_type<T>::count = 0;

#define DOSTRING_EXPECTED(state_, str, expected) \
{                                               \
    try                                         \
    {                                           \
        dostring(state_, str);                  \
    }                                           \
    catch (luabind::error const& e)             \
    {                                           \
		using namespace std;					\
		if (std::strcmp(e.what(),				\
			(char const*)expected))             \
        {                                       \
            TEST_ERROR(e.what());				\
        }                                       \
    }                                           \
    catch (luabind::string const& s)            \
    {                                           \
        if (s != expected)                      \
            TEST_ERROR(s.c_str());              \
    }                                           \
}

#define DOSTRING(state_, str)                   \
{                                               \
    try                                         \
    {                                           \
        dostring(state_, str);                  \
    }                                           \
    catch (luabind::error const& e)             \
    {                                           \
        TEST_ERROR(e.what());					\
    }                                           \
    catch (luabind::string const& s)            \
    {                                           \
        TEST_ERROR(s.c_str());                  \
    }                                           \
}

#endif // TEST_050415_HPP

