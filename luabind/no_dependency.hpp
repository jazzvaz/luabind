// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2010 The Luabind Authors

#pragma once

#include <luabind/detail/policy.hpp>

namespace luabind::detail
{
    struct no_dependency_policy
    {
        static void postcall(lua_State*, int /*results*/, meta::index_list_tag) {}
    };
} // namespace detail

namespace luabind::policy
{
    using no_dependency = policy_list<postcall_injector<detail::no_dependency_policy>>;
} // namespace luabind
