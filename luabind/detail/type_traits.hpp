// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <type_traits>
#include <functional>                // reference_wrapper
#include <luabind/detail/meta.hpp>

namespace luabind::detail
{
    template <typename T>
    using is_const_reference =
        std::bool_constant<std::is_reference_v<T> && std::is_const_v<std::remove_reference_t<T>>>;

    template <typename T>
    constexpr bool is_const_reference_v = is_const_reference<T>::value;

    template <typename T>
    using is_nonconst_reference =
        std::bool_constant<std::is_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>>>;

    template <typename T>
    constexpr bool is_nonconst_reference_v = is_nonconst_reference<T>::value;

    template <typename T>
    using is_const_pointer =
        std::bool_constant<std::is_pointer_v<T> && std::is_const_v<std::remove_pointer_t<T>>>;

    template <typename T>
    constexpr bool is_const_pointer_v = is_const_pointer<T>::value;

    template <typename T>
    using is_nonconst_pointer =
        std::bool_constant<std::is_pointer_v<T> && !std::is_const_v<std::remove_pointer_t<T>>>;

    template <typename T>
    constexpr bool is_nonconst_pointer_v = is_nonconst_pointer<T>::value;

    template <int v1, int v2>
    struct max_c
    {
        static constexpr int value = v1 > v2 ? v1 : v2;
    };
} // namespace luabind::detail

namespace luabind
{
    template <typename T>
    using remove_const_reference_t = std::remove_const_t<std::remove_reference_t<T>>;

    template <typename A, typename B>
    using most_derived_t = std::conditional_t<std::is_base_of_v<A, B>, B, A>;

    struct null_type {};

    template <typename T>
    struct is_null_type : std::false_type
    {};

    template <>
    struct is_null_type<null_type> : std::true_type
    {};

    template <typename T>
    constexpr bool is_null_type_v = is_null_type<T>::value;

    template <typename, typename>
    struct tagged_function;

    template <typename T, typename WrappedType = null_type, typename = void>
    struct deduce_signature;

    template <typename R, typename... Args, typename WrappedType>
    struct deduce_signature<R(Args...), WrappedType>
    {
        using type = meta::type_list<R, Args...>;
    };

    template <typename R, typename... Args, typename WrappedType>
    struct deduce_signature<R(*)(Args...), WrappedType>
    {
        using type = meta::type_list<R, Args...>;
    };

    template <typename R, typename Class, typename... Args>
    struct deduce_signature<R(Class::*)(Args...), null_type>
    {
        using type = meta::type_list<R, Class&, Args...>;
    };

    template <typename R, typename Class, typename... Args>
    struct deduce_signature<R(Class::*)(Args...) const, null_type>
    {
        using type = meta::type_list<R, Class const&, Args...>;
    };

    template <typename R, typename Class, typename... Args, class WrappedType>
    struct deduce_signature<R(Class::*)(Args...), WrappedType>
    {
        using type = meta::type_list<R, most_derived_t<Class, WrappedType>&, Args...>;
    };

    template <typename R, typename Class, typename... Args, class WrappedType>
    struct deduce_signature<R(Class::*)(Args...) const, WrappedType>
    {
        using type = meta::type_list<R, most_derived_t<Class, WrappedType> const&, Args...>;
    };

    template <typename Signature, typename F, class WrappedType>
    struct deduce_signature<tagged_function<Signature, F>, WrappedType>
    {
        using type = Signature;
    };

    template <typename F>
    struct deduce_member_signature;

    template <typename R, typename Class, typename... Args>
    struct deduce_member_signature<R(Class::*)(Args...)>
    {
        using type = meta::type_list<R, Args...>;
    };

    template <typename R, typename Class, typename... Args>
    struct deduce_member_signature<R(Class::*)(Args...) const>
    {
        using type = meta::type_list<R, Args...>;
    };

    template <typename F, typename WrappedType>
    struct deduce_signature<F, WrappedType, std::void_t<decltype(&F::operator())>> :
        deduce_member_signature<decltype(&F::operator())>
    {};

    template <typename T, typename WrappedType = null_type>
    using deduce_signature_t = typename deduce_signature<T, WrappedType>::type;

    template <typename T>
    struct is_reference_wrapper : std::false_type
    {};

    template <typename T>
    struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type
    {};

    template <typename T>
    struct apply_reference_wrapper
    { using type = T; };

    template <typename T>
    struct apply_reference_wrapper<std::reference_wrapper<T>>
    { using type = T&; };

    template <typename T>
    using apply_reference_wrapper_t = typename apply_reference_wrapper<T>::type;

    template <typename T>
    struct identity
    { using type = T; };

    template <typename T>
    using identity_t = typename identity<T>::type;

    template <typename Dst>
    Dst implicit_cast(identity_t<Dst> t) { return t; }
} // namespace luabind
