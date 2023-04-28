// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <functional>
#include <luabind/detail/meta.hpp>
#include <luabind/detail/type_traits.hpp>
#include <luabind/detail/conversion_policies/conversion_base.hpp>
#include <luabind/make_function.hpp>
#include <luabind/detail/call_function.hpp>

namespace luabind
{
    template <typename R = object>
    struct function
    {
        using result_type = R;

        function(luabind::object const& obj) :
            m_func(obj)
        {}

        template <typename... Args>
        R operator() (Args&&... args)
        {
            return call_function<R>(m_func, std::forward<Args>(args)...);
        }

    private:
        object m_func;
    };
} // namespace luabind

namespace luabind::detail
{
    template <typename T>
    constexpr bool is_function_pointer =
        std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

    template <typename T, typename = void>
    struct is_function : std::false_type
    {};

    template <typename T>
    struct is_function<T, std::enable_if_t<is_function_pointer<T>>> : std::true_type
    {};

    template <typename T>
    struct is_function<T, std::void_t<decltype(&T::operator())>> : std::true_type
    {};

    template <typename T>
    constexpr bool is_function_v = is_function<T>::value;
} // namespace luabind::detail

namespace luabind
{
    template <typename F>
    struct default_converter<F, std::enable_if_t<detail::is_function_v<remove_const_reference_t<F>>>>
    {
        using is_native = std::true_type;

        static constexpr int consumed_args = 1;

        template <class U>
        void converter_postcall(lua_State*, U const&, int) {}

        template <class U>
        int match(lua_State* L, U, int index)
        {
            if (lua_type(L, index) == LUA_TFUNCTION)
                return 0;
            if (luaL_getmetafield(L, index, "__call"))
            {
                lua_pop(L, 1);
                return 1;
            }
            return no_match;
        }

        template <class U>
        F to_cpp(lua_State* L, U, int index)
        {
            // If you get a compiler error here, you are probably trying to
            // get a function pointer from Lua. This is not supported:
            // you must use a type which is constructible from a
            // luabind::function, e.g. std::function or boost::function.
            return function<typename F::result_type>(object(from_stack(L, index)));
        }

        void to_lua(lua_State* L, F value)
        {
            make_function(L, value, false).push(L);
        }
    };
} // namespace luabind
