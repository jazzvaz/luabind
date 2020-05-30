// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind::detail
{
    struct yield_policy
    {
        inline static void postcall(lua_State*, int /*results*/, meta::index_list_tag) {}
    };
} // namespace luabind::detail

namespace luabind::policy
{
    using yield = postcall_injector<detail::yield_policy>;
} // namespace luabind::policy
