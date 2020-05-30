// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include <luabind/detail/type_traits.hpp>

using namespace luabind;
using namespace luabind::detail;

struct tester {};
struct lua_State;

#define LUABIND_STATIC_ASSERT(expr) static_assert(expr, #expr)

LUABIND_STATIC_ASSERT(is_nonconst_reference<int&>::value);
LUABIND_STATIC_ASSERT(!is_nonconst_reference<const int&>::value);
LUABIND_STATIC_ASSERT(is_nonconst_reference<tester&>::value);
LUABIND_STATIC_ASSERT(!is_nonconst_reference<const tester&>::value);

LUABIND_STATIC_ASSERT(!is_const_reference<int&>::value);
LUABIND_STATIC_ASSERT(is_const_reference<const int&>::value);
LUABIND_STATIC_ASSERT(!is_const_reference<tester&>::value);
LUABIND_STATIC_ASSERT(is_const_reference<const tester&>::value);

LUABIND_STATIC_ASSERT(!is_const_pointer<int*>::value);
LUABIND_STATIC_ASSERT(is_const_pointer<const int*>::value);
LUABIND_STATIC_ASSERT(!is_const_pointer<tester*>::value);
LUABIND_STATIC_ASSERT(is_const_pointer<const tester*>::value);

LUABIND_STATIC_ASSERT(is_nonconst_pointer<int*>::value);
LUABIND_STATIC_ASSERT(!is_nonconst_pointer<const int*>::value);
LUABIND_STATIC_ASSERT(is_nonconst_pointer<tester*>::value);
LUABIND_STATIC_ASSERT(!is_nonconst_pointer<const tester*>::value);

LUABIND_STATIC_ASSERT(!is_const_reference<const tester>::value);
