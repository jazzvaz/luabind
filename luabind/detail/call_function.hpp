// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

#include <luabind/error.hpp>
#include <luabind/lua_stack.hpp>
#include <luabind/detail/pcall.hpp>
#include <luabind/detail/call_shared.hpp>
#include <luabind/detail/stack_utils.hpp>

namespace luabind
{
    namespace adl
    {
        class object;
    } // namespace adl

    using adl::object;
} // namespace luabind

namespace luabind::detail
{
    template <typename PolicyList, uint32_t pos>
    void push_arguments(lua_State* /*L*/) {}

    template <typename PolicyList, uint32_t Pos, typename Arg0, typename... Args>
    void push_arguments(lua_State* L, Arg0&& arg0, Args&&... args)
    {
        using converter_type = specialized_converter_policy<
            fetched_converter_policy<Pos, PolicyList>, Arg0, cpp_to_lua>;
        converter_type().to_lua(L, unwrapped<Arg0>::get(std::forward<Arg0>(arg0)));
        push_arguments<PolicyList, Pos + 1>(L, std::forward<Args>(args)...);
    }

    using lua_fun = int(*)(lua_State*, int, int);

    template <typename PolicyList, typename IndexList, uint32_t NArgs, typename Ret, typename... Args>
    Ret call_function(lua_State* L, lua_fun function, Args&&... args)
    {
        int top = lua_gettop(L);
        push_arguments<PolicyList, 1>(L, std::forward<Args>(args)...);
        if (function(L, sizeof...(Args), 1))
        {
            if (function == &detail::pcall)
                assert(lua_gettop(L) == static_cast<int>(top - NArgs + 1));
            call_error(L);
        }
        // pops the return values from the function call
        stack_pop pop(L, lua_gettop(L) - top + NArgs);
        if constexpr (!std::is_void_v<Ret>)
        {
            specialized_converter_policy_n<0, PolicyList, Ret, lua_to_cpp> converter;
            if (converter.match(L, decorate_type_t<Ret>(), -1) < 0 && !get_permissive_mode())
                cast_error<Ret>(L);
            return converter.to_cpp(L, decorate_type_t<Ret>(), -1);
        }
    }
} // namespace luabind::detail

namespace luabind
{
    template <class R, typename PolicyList = no_policies, typename... Args>
    R call_pushed_function(lua_State* L, Args&&... args)
    {
        using index_list = meta::index_range<1, sizeof...(Args)+1>;
        return detail::call_function<PolicyList, index_list, 1, R>(
            L, &detail::pcall, std::forward<Args>(args)...);
    }

    template <class R, typename PolicyList = no_policies, typename... Args>
    R call_function(lua_State* L, const char* name, Args&&... args)
    {
        assert(name && "luabind::call_function() expects a function name");
        lua_getglobal(L, name);
        return call_pushed_function<R, PolicyList>(L, std::forward<Args>(args)...);
    }

    template <class R, typename PolicyList = no_policies, typename... Args>
    R resume_pushed_function(lua_State* L, Args&&... args)
    {
        using index_list = meta::index_range<1, sizeof...(Args)+1>;
        return detail::call_function<PolicyList, index_list, 1, R>(
            L, &detail::resume_impl, std::forward<Args>(args)...);
    }

    template <class R, typename PolicyList = no_policies, typename... Args>
    R resume_function(lua_State* L, const char* name, Args&&... args)
    {
        assert(name && "luabind::resume_function() expects a function name");
        lua_getglobal(L, name);
        return resume_pushed_function<R, PolicyList>(L, std::forward<Args>(args)...);
    }

    template <class R, typename PolicyList = no_policies, typename... Args>
    R resume(lua_State* L, Args&&... args)
    {
        using index_list = meta::index_range<1, sizeof...(Args)+1>;
        return detail::call_function<PolicyList, index_list, 0, R>(
            L, &detail::resume_impl, std::forward<Args>(args)...);
    }
} // namespace luabind
