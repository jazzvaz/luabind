// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/lua_state_fwd.hpp>

namespace luabind::detail
{
    LUABIND_API int pcall(lua_State *L, int nargs, int nresults);
    LUABIND_API int resume_impl(lua_State *L, int nargs, int nresults);
} // namespace luabind::detail
