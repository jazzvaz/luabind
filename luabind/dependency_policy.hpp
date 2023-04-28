// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>    // for policy_cons, etc
#include <luabind/detail/object_rep.hpp>  // for object_rep
#include <luabind/detail/primitives.hpp>  // for null_type

namespace luabind::detail
{
    // makes A dependent on B, meaning B will outlive A.
    // internally A stores a reference to B
    template <int A, int B>
    struct dependency_policy
    {
        template <uint32_t... StackIndices>
        static void postcall(lua_State* L, int results, meta::index_list<StackIndices...>)
        {
            void* ud = lua_touserdata(L, meta::get_v<meta::index_list<StackIndices...>, A>);
            auto* nurse = static_cast<object_rep*>(ud);
            // If the nurse isn't an object_rep, just make this a nop.
            // XXX: this will crash if nurse is not object_rep pointer (same as below postcalls)
            if (nurse)
                nurse->add_dependency(L, meta::get_v<meta::index_list<StackIndices...>, B>);
        }
    };

    template <int B>
    struct dependency_policy<0, B>
    {
        template <uint32_t... StackIndices>
        static void postcall(lua_State* L, int results, meta::index_list<StackIndices...>)
        {
            void* ud = lua_touserdata(L, meta::get_v<meta::index_list<StackIndices...>, 0> +results);
            auto* nurse = static_cast<object_rep*>(ud);
            if (nurse)
                nurse->add_dependency(L, meta::get_v<meta::index_list<StackIndices...>, B>);
        }
    };

    template <int A>
    struct dependency_policy<A, 0>
    {
        template <uint32_t... StackIndices>
        static void postcall(lua_State* L, int results, meta::index_list<StackIndices...>)
        {
            void* ud = lua_touserdata(L, meta::get_v<meta::index_list<StackIndices...>, A>);
            auto* nurse = static_cast<object_rep*>(ud);
            if (nurse)
                nurse->add_dependency(L, meta::get_v<meta::index_list<StackIndices...>, 0> +results);
        }
    };
} // namespace luabind::detail

namespace luabind::policy
{
    template <uint32_t A, uint32_t B>
    using dependency = postcall_injector<detail::dependency_policy<A, B>>;

    template <uint32_t A>
    using return_ref = postcall_injector<detail::dependency_policy<0, A>>;
} // namespace luabind::policy
