// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

namespace luabind::detail
{
    enum operator_id
    {
        op_add = 0,
        op_sub,
        op_mul,
        op_div,
        op_mod,
        op_pow,
        op_lt,
        op_le,
        op_eq,
        op_call,
        op_unm,
        op_tostring,
        op_concat,
        op_len,
        number_of_operators
    };

    inline const char* get_operator_name(int i)
    {
        static const char* a[number_of_operators] =
        {
            "__add", "__sub", "__mul", "__div", "__mod", "__pow",
            "__lt", "__le", "__eq", "__call", "__unm",
            "__tostring", "__concat", "__len"
        };
        return a[i];
    }

    inline const char* get_operator_symbol(int i)
    {
        static const char* a[number_of_operators] =
        {
            "+", "-", "*", "/", "%", "^", "<",
            "<=", "==", "()", "- (unary)",
            "tostring", "..", "#"
        };
        return a[i];
    }

    inline bool is_unary(int i)
    {
        // the reason why unary minus is not considered a unary operator here is
        // that it always is given two parameters, where the second parameter always
        // is nil.
        return i == op_tostring;
    }
} // namespace luabind::detail
