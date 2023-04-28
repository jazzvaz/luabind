// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <luabind/lua_proxy.hpp>
#include <luabind/detail/call_function.hpp>
#include <luabind/lua_stack.hpp>
#include <ostream>
#include <optional>

namespace luabind::adl
{
    template <class T>
    class lua_proxy_interface;

    struct object_checker
    {
        template <class T>
        std::true_type operator()(lua_proxy_interface<T>*);
        std::false_type operator()(void*);
    };

    template <class T>
    struct is_object_interface : decltype(object_checker()((remove_const_reference_t<T>*)nullptr))
    {};

    template <class T>
    constexpr bool is_object_interface_v = is_object_interface<T>::value;

    template <class R, class T, class U>
    struct enable_binary : std::enable_if<is_object_interface_v<T> || is_object_interface_v<U>, R>
    {};

    template <class R, class T, class U>
    using enable_binary_t = typename enable_binary<R, T, U>::type;

    template <class T, class U>
    int binary_interpreter(lua_State*& L, T const& x, U const& y)
    {
        if constexpr (is_lua_proxy_type_v<T> && is_lua_proxy_type_v<U>)
        {
            L = lua_proxy_traits<T>::interpreter(x);
            lua_State* L2 = lua_proxy_traits<U>::interpreter(y);
            // you are comparing objects with different interpreters
            // that's not allowed.
            assert(L == L2 || !L || !L2);
            // if the two objects we compare have different interpreters
            // then they
            if (L != L2)
                return -1;
            if (!L)
                return 1;
        }
        else if constexpr (is_lua_proxy_type_v<T>)
            L = lua_proxy_traits<T>::interpreter(x);
        else if constexpr (is_lua_proxy_type_v<U>)
            L = lua_proxy_traits<U>::interpreter(y);
        else
            static_assert(!sizeof(T*), "both types must be lua proxy types");
        return 0;
    }

    template <class LHS, class RHS>
    enable_binary_t<bool, LHS, RHS> operator==(LHS&& lhs, RHS&& rhs)
    {
        lua_State* L = 0;
        switch (binary_interpreter(L, lhs, rhs))
        {
        case 1: return true;
        case -1: return false;
        }
        assert(L);
        detail::stack_pop pop1(L, 1);
        lua_stack::push(L, std::forward<LHS>(lhs));
        detail::stack_pop pop2(L, 1);
        lua_stack::push(L, std::forward<RHS>(rhs));
        return lua_compare(L, -1, -2, LUA_OPEQ) != 0;
    }

    template <class LHS, class RHS>
    enable_binary_t<bool, LHS, RHS> operator<(LHS&& lhs, RHS&& rhs)
    {
        lua_State* L = nullptr;
        switch (binary_interpreter(L, lhs, rhs))
        {
        case 1: return true;
        case -1: return false;
        }
        assert(L);
        detail::stack_pop pop1(L, 1);
        lua_stack::push(L, std::forward<LHS>(lhs));
        detail::stack_pop pop2(L, 1);
        lua_stack::push(L, std::forward<RHS>(rhs));
        return lua_compare(L, -1, -2, LUA_OPLT) != 0;
    }

    inline int value_to_string(lua_State* L)
    {
        assert(lua_gettop(L) == 1);
        luaL_tolstring(L, 1, 0);
        return 1;
    }

    template <class ValueWrapper>
    std::ostream& operator<<(std::ostream& os, lua_proxy_interface<ValueWrapper> const& v)
    {
        using namespace luabind;
        lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(
            static_cast<ValueWrapper const&>(v));
        detail::stack_pop pop(L, 1);
        lua_pushcfunction(L, &value_to_string);
        lua_proxy_traits<ValueWrapper>::unwrap(L, static_cast<ValueWrapper const&>(v));
        if (lua_pcall(L, 1, 1, 0) != LUA_OK)
        {
            lua_pop(L, 1); // Pop error.
            os.setstate(std::ios::failbit);
            return os;
        }
        size_t len;
        char const* p = lua_tolstring(L, -1, &len);
        os.write(p, len);
        return os;
    }

    template <class LHS, class RHS>
    enable_binary_t<bool, LHS, RHS> operator>(LHS const& lhs, RHS const& rhs)
    {
        return !(lhs < rhs || lhs == rhs);
    }

    template <class LHS, class RHS>
    enable_binary_t<bool, LHS, RHS> operator<=(LHS const& lhs, RHS const& rhs)
    {
        return lhs < rhs || lhs == rhs;
    }

    template <class LHS, class RHS>
    enable_binary_t<bool, LHS, RHS> operator>=(LHS const& lhs, RHS const& rhs)
    {
        return !(lhs < rhs);
    }

    template <class LHS, class RHS>
    enable_binary_t<bool, LHS, RHS> operator!=(LHS const& lhs, RHS const& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Derived>
    class lua_proxy_interface
    {
    public:
        ~lua_proxy_interface() {}

        // defined in luabind/detail/object.hpp
        template <typename... Args>
        object operator()(Args&&... args);

        // defined in luabind/detail/object.hpp
        template <typename PolicyList, typename... Args>
        object call(Args&&... args);

        explicit operator bool() const
        {
            lua_State* L = lua_proxy_traits<Derived>::interpreter(derived());
            if (!L)
                return false;
            lua_proxy_traits<Derived>::unwrap(L, derived());
            detail::stack_pop pop(L, 1);
            return lua_toboolean(L, -1) == 1;
        }

    private:
        Derived& derived()
        {
            return *static_cast<Derived*>(this);
        }
        Derived const& derived() const
        {
            return *static_cast<Derived const*>(this);
        }
    };
} // namespace luabind::adl

namespace luabind
{
    template <class ValueWrapper>
    luabind::string to_string(adl::lua_proxy_interface<ValueWrapper> const& v)
    {
        using namespace luabind;
        lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(static_cast<ValueWrapper const&>(v));
        detail::stack_pop pop(L, 1);
        lua_proxy_traits<ValueWrapper>::unwrap(L, static_cast<ValueWrapper const&>(v));
        char const* p = lua_tostring(L, -1);
        std::size_t len = lua_rawlen(L, -1);
        return luabind::string(p, len);
    }

    namespace detail
    {
        template <class T, class ReturnType, class Policies, class ValueWrapper, class ErrorPolicy>
        ReturnType object_cast_aux(ValueWrapper const& value_wrapper, ErrorPolicy error_policy)
        {
            lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(value_wrapper);
#ifndef LUABIND_NO_ERROR_CHECKING
            if (!L)
                return error_policy.handle_error(L, typeid(void));
#endif
            lua_proxy_traits<ValueWrapper>::unwrap(L, value_wrapper);
            detail::stack_pop pop(L, 1);
            specialized_converter_policy_n<0, Policies, T, lua_to_cpp> cv;
            if (cv.match(L, decorate_type_t<T>(), -1) < 0)
                return error_policy.handle_error(L, typeid(T));
            return cv.to_cpp(L, decorate_type_t<T>(), -1);
        }

        template <class T>
        struct throw_error_policy
        {
            T handle_error(lua_State* interpreter, type_id const& type_info)
            {
#ifndef LUABIND_NO_EXCEPTIONS
                throw cast_failed(interpreter, type_info);
#else
                cast_failed_callback_fun e = get_cast_failed_callback();
                if (e)
                    e(interpreter, type_info);
                assert(!"object_cast failed. If you want to handle this error use "
                    "luabind::set_error_callback()");
                std::terminate();
#endif
            }
        };

        template <class T>
        struct nothrow_error_policy
        {
            nothrow_error_policy(T rhs) :
                value(rhs)
            {}

            T handle_error(lua_State*, type_id const&)
            {
                return value;
            }
        private:
            T value;
        };
    } // namespace detail

    template <class T, class ValueWrapper>
    T object_cast(ValueWrapper const& wrapper)
    {
        return detail::object_cast_aux<T, T, no_policies>(wrapper, detail::throw_error_policy<T>());
    }

    template <class T, class ValueWrapper, class Policies>
    T object_cast(ValueWrapper const& wrapper, Policies const&)
    {
        return detail::object_cast_aux<T, T, Policies>(wrapper, detail::throw_error_policy<T>());
    }

    template <typename T, typename ValueWrapper>
    std::optional<T> object_cast_nothrow(ValueWrapper const& wrapper)
    {
        return detail::object_cast_aux<T, std::optional<T>, no_policies>(
            wrapper, detail::nothrow_error_policy(std::nullopt));
    }

    template <typename T, typename ValueWrapper, typename Policies>
    std::optional<T> object_cast_nothrow(ValueWrapper const& wrapper, Policies const&)
    {
        return detail::object_cast_aux<T, std::optional<T>, Policies>(
            wrapper, detail::nothrow_error_policy(std::nullopt));
    }

    template <class ValueWrapper>
    lua_CFunction tocfunction(ValueWrapper const& value)
    {
        lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(value);
        lua_proxy_traits<ValueWrapper>::unwrap(L, value);
        detail::stack_pop pop(L, 1);
        return lua_tocfunction(L, -1);
    }

    template <class T, class ValueWrapper>
    T* touserdata(ValueWrapper const& value)
    {
        lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(value);
        lua_proxy_traits<ValueWrapper>::unwrap(L, value);
        detail::stack_pop pop(L, 1);
        return static_cast<T*>(lua_touserdata(L, -1));
    }
} // namespace luabind
