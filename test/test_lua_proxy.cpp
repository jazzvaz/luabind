// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#include <luabind/lua_proxy.hpp>
#include <luabind/object.hpp>

struct X_tag;

struct X
{
    typedef X_tag lua_proxy_tag;
};

namespace luabind
{
  template<>
  struct lua_proxy_traits<X>
  {
      typedef std::true_type is_specialized;
  };
} // namespace luabind

#define LUABIND_STATIC_ASSERT(expr) static_assert(expr, #expr)

LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_type<X>::value);
LUABIND_STATIC_ASSERT(!luabind::is_lua_proxy_type<X&>::value);
LUABIND_STATIC_ASSERT(!luabind::is_lua_proxy_type<X const&>::value);

LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_arg<X>::value);
LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_arg<X const>::value);
LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_arg<X&>::value);
LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_arg<X const&>::value);
LUABIND_STATIC_ASSERT(!luabind::is_lua_proxy_arg<int>::value);
LUABIND_STATIC_ASSERT(!luabind::is_lua_proxy_arg<int[4]>::value);

LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_arg<X const&>::value);
LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_arg<luabind::object&>::value);
LUABIND_STATIC_ASSERT(luabind::is_lua_proxy_arg<luabind::object const&>::value);
