// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/type_traits.hpp>
#include <luabind/detail/stack_utils.hpp>
#include <luabind/error.hpp> // call_shared
#include <luabind/detail/call_shared.hpp> // cast_error

namespace luabind::detail
{
    template <typename T>
    struct unwrapped
    {
        static constexpr bool is_wrapped_ref = false;
        using type = T;

        static const T& get(const T& t)
        {
            return t;
        }
    };

    template <typename T>
    struct unwrapped<std::reference_wrapper<T>>
    {
        static constexpr bool is_wrapped_ref = true;
        using type = T&;

        static T& get(const std::reference_wrapper<T>& refwrap)
        {
            return refwrap.get();
        }
    };

    template <typename T>
    using unwrapped_t = typename unwrapped<T>::type;
} // namespace luabind::detail

namespace luabind::lua_stack
{
    template <typename Policies = no_policies, int PolicyIndex = 1, typename T>
    void push(lua_State* L, T&& v)
    {
        using namespace detail;
        using value_type = unwrapped_t<remove_const_reference_t<T>>;
        specialized_converter_policy_n<PolicyIndex, Policies, value_type, cpp_to_lua> cv;
        cv.to_lua(L, unwrapped<T>::get(v));
    }

    template <typename T, typename Policies = no_policies, int PolicyIndex = 1>
    T pop(lua_State* L)
    {
        using namespace detail;
        using value_type = unwrapped_t<T>;
        stack_pop pop(L, 1);
        specialized_converter_policy_n<PolicyIndex, Policies, value_type, lua_to_cpp> cv;
#ifndef LUABIND_ALLOW_MISSING_ARGUMENTS
        if (cv.match(L, decorate_type_t<T>(), -1) < 0)
            cast_error<T>(L);
#endif // !LUABIND_ALLOW_MISSING_ARGUMENTS
        return cv.to_cpp(L, decorate_type_t<T>(), -1);
    }

    template <>
    inline void pop<void, no_policies, 1>(lua_State* L)
    {
        detail::stack_pop pop(L, 1);
    }

    inline void pop(lua_State* L, int n = 1)
    {
        detail::stack_pop pop(L, n);
    }

} // namespace luabind::lua_stack
