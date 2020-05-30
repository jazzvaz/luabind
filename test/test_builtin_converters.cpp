// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

template <typename T>
void test_converter(int value)
{
    using namespace luabind;
    default_converter<T>().to_lua_deferred(L, value);
    default_converter<T> cv;
    CHECK(cv.compute_score(L, -1) >= 0);
    CHECK(cv.to_cpp_deferred(L, -1) == value);
    lua_pop(L, 1);
}

TEST_CASE("builtin_converters")
{
    test_converter<int>(1);
    test_converter<int>(-1);
    test_converter<unsigned int>(1);
    test_converter<unsigned int>(2);

    test_converter<short>(1);
    test_converter<short>(-1);
    test_converter<unsigned short>(1);
    test_converter<unsigned short>(2);

    test_converter<long>(1);
    test_converter<long>(-1);
    test_converter<unsigned long>(1);
    test_converter<unsigned long>(2);

    test_converter<char>(1);
    test_converter<char>(2);
    test_converter<unsigned char>(1);
    test_converter<unsigned char>(2);
    test_converter<signed char>(-1);
    test_converter<signed char>(1);

    test_converter<float>(1.5);
    test_converter<float>(-1.5);

    test_converter<double>(1.5);
    test_converter<double>(-1.5);

    test_converter<bool>(true);
    test_converter<bool>(false);
}
