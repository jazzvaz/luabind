// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

// header derived from source code found in Boost.Python

// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <luabind/config.hpp>

namespace luabind
{
    template <class T>
    struct other
    {
        using type = T;
    };
} // namespace luabind

namespace luabind::detail
{
    template <typename T>
    struct unwrap_other
    {
        using type = T;
    };

    template <typename T>
    struct unwrap_other<other<T>>
    {
        using type = T;
    };

    template <typename T>
    using unwrap_other_t = typename unwrap_other<T>::type;
} // namespace luabind::detail
