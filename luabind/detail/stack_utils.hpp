// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#pragma once

#ifndef LUA_INCLUDE_HPP_INCLUDED
#include <luabind/lua_include.hpp>
#endif

#include <cassert>

namespace luabind {
	namespace detail {

		struct stack_pop
		{
			stack_pop(lua_State* L, int n)
				: m_state(L)
				, m_n(n)
			{
			}

			~stack_pop()
			{
				lua_pop(m_state, m_n);
			}

		private:

			lua_State* m_state;
			int m_n;
		};

	} // namespace detail
} // namespace luabind
