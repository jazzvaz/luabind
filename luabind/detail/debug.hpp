// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#ifdef DEBUG

#include <luabind/lua_include.hpp>
#include <cassert>

namespace luabind::detail
{
    struct stack_checker_type
    {
        stack_checker_type(lua_State* L) :
            m_L(L),
            m_stack(lua_gettop(m_L))
        {}

        ~stack_checker_type()
        {
            assert(m_stack == lua_gettop(m_L));
        }

        lua_State* m_L;
        int m_stack;
    };
} // namespace luabind::detail

# define LUABIND_CHECK_STACK(L) luabind::detail::stack_checker_type stack_checker_object(L)
#else
# define LUABIND_CHECK_STACK(L) do {} while (0)
#endif
