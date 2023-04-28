// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

namespace luabind
{
    template <class T>
    struct by_value {};

    template <class T>
    struct by_const_reference {};

    template <class T>
    struct by_reference {};

    template <class T>
    struct by_rvalue_reference {};

    template <class T>
    struct by_pointer {};

    template <class T>
    struct by_const_pointer {};

    template <class T>
    struct decorate_type
    {
        using type = by_value<T>;
    };

    template <class T>
    struct decorate_type<T*>
    {
        using type = by_pointer<T>;
    };

    template <class T>
    struct decorate_type<const T*>
    {
        using type = by_const_pointer<T>;
    };

    template <class T>
    struct decorate_type<const T* const>
    {
        using type = by_const_pointer<T>;
    };

    template <class T>
    struct decorate_type<T&>
    {
        using type = by_reference<T>;
    };

    template <class T>
    struct decorate_type<const T&>
    {
        using type = by_const_reference<T>;
    };

    template <class T>
    struct decorate_type<T&&>
    {
        using type = by_rvalue_reference<T>;
    };

    template <typename T>
    using decorate_type_t = typename decorate_type<T>::type;
} // namespace luabind
