// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <luabind/make_function.hpp>
#include <luabind/scope.hpp>
#include <luabind/detail/call_function.hpp>

namespace luabind::detail
{
    template <class F, class PolicyList>
    struct function_registration : registration
    {
        function_registration(char const* name, F f) :
            name(name),
            f(f)
        {}

        void register_(lua_State* L, bool default_scope = false) const
        {
            object fn = make_function(L, f, default_scope, PolicyList());
            add_overload(object(from_stack(L, -1)), name, fn);
        }

        char const* name;
        F f;
    };

    LUABIND_API bool is_luabind_function(lua_State* L, int index, bool allow_default = true);
} // namespace luabind::detail

namespace luabind
{
    template <class F, typename... Policies>
    scope def(char const* name, F f, policy_list<Policies...> const&)
    {
        using PolicyList = policy_list<Policies...>;
        auto reg = luabind_new<detail::function_registration<F, PolicyList>>(name, f);
        return scope(luabind::unique_ptr<detail::registration>(reg));
    }

    template <class F>
    scope def(char const* name, F f)
    { return def(name, f, no_policies()); }
} // namespace luabind
