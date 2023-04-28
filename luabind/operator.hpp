// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#pragma once

#if defined(__GNUC__) && __GNUC__ < 3
# define LUABIND_NO_STRINGSTREAM
#else
# if defined(BOOST_NO_STRINGSTREAM)
#  define LUABIND_NO_STRINGSTREAM
# endif
#endif

#ifdef LUABIND_NO_STRINGSTREAM
#include <strstream>
#else
#include <sstream>
#endif

#include <luabind/detail/meta.hpp>
#include <luabind/lua_include.hpp>
#include <luabind/detail/other.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind::detail
{
    template <class W, class T>
    struct unwrap_arg_type;

    template <class W, class T>
    using unwrap_arg_type_t = typename unwrap_arg_type<W, T>::type;

    template <class Derived> struct operator_
    {};

    struct operator_void_return
    {};

    template <class T>
    T const& operator,(T const& x, operator_void_return) { return x; }

    template <class Policies>
    void operator_result(lua_State*, operator_void_return) {}

    template <class Policies, class T>
    void operator_result(lua_State* L, T const& x)
    {
        specialized_converter_policy_n<0, Policies, T, cpp_to_lua>().to_lua(L, x);
    }
} // namespace luabind::detail

namespace luabind::operators
{
    template <class Self, typename... Args>
    struct call_operator : detail::operator_<call_operator<Self, Args...>>
    {
        call_operator(int) {}

        template <class T, class Policies>
        struct apply
        {
            static void execute(lua_State* L,
                detail::unwrap_arg_type_t<T, Self> self,
                detail::unwrap_arg_type_t<T, Args>... args)
            {
                using namespace detail;
                operator_result<Policies>(L, (self(args...), operator_void_return()));
            }
        };

        static char const* name() { return "__call"; }
    };
} // namespace luabind::operators

namespace luabind
{
    template <class Derived>
    struct self_base
    {
        template <typename... Args>
        operators::call_operator<Derived, Args...> operator()(const Args&...) const
        {
            return 0;
        }
    };

    struct self_type : self_base<self_type>
    {};

    struct const_self_type : self_base<const_self_type>
    {};
} // namespace luabind

namespace luabind::detail
{
    template <class W, class T>
    struct unwrap_arg_type
    {
        using type = meta::select_t<
            meta::case_<std::is_same<T, self_type>, W&>,
            meta::case_<std::is_same<T, const_self_type>, W const&>,
            meta::default_<unwrap_other_t<T>>>;
    };

    template <class Derived, class A, class B>
    struct binary_operator : operator_<binary_operator<Derived, A, B>>
    {
        binary_operator(int) {}

        template <class T, class Policies>
        struct apply
        {
            using arg0 = unwrap_arg_type_t<T, A>;
            using arg1 = unwrap_arg_type_t<T, B>;

            static void execute(lua_State* L, arg0 _0, arg1 _1)
            {
                Derived::template apply<arg0, arg1, Policies>::execute(L, _0, _1);
            }
        };

        static char const* name() { return Derived::name(); }
    };

    template <class Derived, class A>
    struct unary_operator : operator_<unary_operator<Derived, A>>
    {
        unary_operator(int) {}

        template <class T, class Policies>
        struct apply
        {
            using arg0 = unwrap_arg_type_t<T, A>;

            static void execute(lua_State* L, arg0 _0)
            {
                Derived::template apply<arg0, Policies>::execute(L, _0);
            }
        };

        static char const* name() { return Derived::name(); }
    };
} // namespace luabind::detail

namespace luabind
{
#define LUABIND_BINARY_OPERATOR(name_, op) \
    namespace operators { \
        struct name_ \
        { \
            template <class T0, class T1, class Policies> \
            struct apply \
            { \
                static void execute(lua_State* L, T0 _0, T1 _1) \
                { detail::operator_result<Policies>(L, _0 op _1); } \
            }; \
            static char const* name() { return "__" #name_; } \
        }; \
    } \
    template <class T, class U> \
    detail::binary_operator<operators::name_, U, T> \
    inline operator op(self_base<U>, T const&) \
    { return 0; } \
    template <class T, class U> \
    detail::binary_operator<operators::name_, T, U> \
    inline operator op(T const&, self_base<U>) \
    { return 0; } \
    detail::binary_operator<operators::name_, self_type, self_type> \
    inline operator op(self_type, self_type) \
    { return 0; } \
    detail::binary_operator<operators::name_, self_type, const_self_type> \
    inline operator op(self_type, const_self_type) \
    { return 0; } \
    detail::binary_operator<operators::name_, const_self_type, self_type> \
    inline operator op(const_self_type, self_type) \
    { return 0; } \
    detail::binary_operator<operators::name_, const_self_type, const_self_type> \
    inline operator op(const_self_type, const_self_type) \
    { return 0; }

    LUABIND_BINARY_OPERATOR(add, +)
        LUABIND_BINARY_OPERATOR(sub, -)
        LUABIND_BINARY_OPERATOR(mul, *)
        LUABIND_BINARY_OPERATOR(div, /)
        LUABIND_BINARY_OPERATOR(mod, %)
        LUABIND_BINARY_OPERATOR(pow, ^)
        LUABIND_BINARY_OPERATOR(lt, <)
        LUABIND_BINARY_OPERATOR(le, <=)
        LUABIND_BINARY_OPERATOR(gt, >)
        LUABIND_BINARY_OPERATOR(ge, >=)
        LUABIND_BINARY_OPERATOR(eq, ==)

#undef LUABIND_BINARY_OPERATOR

#define LUABIND_UNARY_OPERATOR(name_, op, fn) \
    namespace operators { \
        struct name_ \
        { \
            template <class T, class Policies> \
            struct apply \
            { \
                static void execute(lua_State* L, T x) \
                { detail::operator_result<Policies>(L, op(x)); } \
            }; \
            static char const* name() { return "__" #name_; } \
        }; \
    } \
    template <class T> \
    inline detail::unary_operator<operators::name_, T> fn(self_base<T>) \
    { return 0; }

        template <class T>
    luabind::string tostring_operator(T const& x)
    {
#ifdef LUABIND_NO_STRINGSTREAM
        std::strstream s;
        s << x << std::ends;
#else
        luabind::stringstream s;
        s << x;
#endif
        return s.str();
    }

    LUABIND_UNARY_OPERATOR(tostring, tostring_operator, tostring)
        LUABIND_UNARY_OPERATOR(unm, -, operator-)
#undef LUABIND_UNARY_OPERATOR

        extern LUABIND_API self_type self;
    extern LUABIND_API const_self_type const_self;
} // namespace luabind
