// Copyright (c) 2004 Daniel Wallin and Arvid Norberg

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#define LUABIND_BUILDING

#include <luabind/lua_include.hpp>

#include <luabind/scope.hpp>
#include <luabind/detail/debug.hpp>
#include <luabind/detail/stack_utils.hpp>
#include <cassert>

namespace luabind {
	namespace detail {

		registration::registration()
			: m_next(0)
		{
		}

		registration::~registration()
		{
			luabind_delete(m_next);
		}

	} // namespace detail

	scope::scope()
		: m_chain(0)
	{
	}

	scope::scope(luabind::unique_ptr<detail::registration> reg)
		: m_chain(reg.release())
	{
	}

	scope::scope(scope const& other)
		: m_chain(other.m_chain)
	{
		const_cast<scope&>(other).m_chain = 0;
	}

	scope& scope::operator=(scope const& other_)
	{
		luabind_delete(m_chain);
		m_chain = other_.m_chain;
		const_cast<scope&>(other_).m_chain = 0;
		return *this;
	}

	scope::~scope()
	{
		luabind_delete(m_chain);
	}

	scope& scope::operator,(scope s)
	{
		if(!m_chain)
		{
			m_chain = s.m_chain;
			s.m_chain = 0;
			return *this;
		}

		for(detail::registration* c = m_chain;; c = c->m_next)
		{
			if(!c->m_next)
			{
				c->m_next = s.m_chain;
				s.m_chain = 0;
				break;
			}
		}

		return *this;
	}

	void scope::register_(lua_State* L, bool default_scope /*= false*/) const
	{
		for(detail::registration* r = m_chain; r != 0; r = r->m_next)
		{
			LUABIND_CHECK_STACK(L);
			r->register_(L, default_scope);
		}
	}

	namespace {

		struct lua_pop_stack
		{
			lua_pop_stack(lua_State* L)
				: m_state(L)
			{
			}

			~lua_pop_stack()
			{
				lua_pop(m_state, 1);
			}

			lua_State* m_state;
		};

	} // namespace unnamed

	module_::module_(object const& table)
		: m_table(table)
	{
	}

	module_::module_(lua_State* L, char const* name)
	{
		if(name)
		{
			lua_getglobal(L, name);

			if(!lua_istable(L, -1))
			{
				lua_pop(L, 1);

				lua_newtable(L);
				lua_pushvalue(L, -1);
				lua_setglobal(L, name);
			}
		}
		else
		{
			lua_pushglobaltable(L);
		}

		m_table = object(from_stack(L, -1));
		lua_pop(L, 1);
	}

	void module_::operator[](scope s)
	{
		lua_State* L = m_table.interpreter();
		m_table.push(L);
		lua_pop_stack guard(L);
		s.register_(L);
	}

	struct namespace_::registration_ : detail::registration
	{
		registration_(char const* name)
			: m_name(name)
		{
		}

		void register_(lua_State* L, bool /*default_scope = false*/) const
		{
			LUABIND_CHECK_STACK(L);
			assert(lua_gettop(L) >= 1);

			lua_pushstring(L, m_name);
			lua_gettable(L, -2);

			detail::stack_pop p(L, 1); // pops the table on exit

			if(!lua_istable(L, -1))
			{
				lua_pop(L, 1);

				lua_newtable(L);
				lua_pushstring(L, m_name);
				lua_pushvalue(L, -2);
				lua_settable(L, -4);
			}

			m_scope.register_(L);
		}

		char const* m_name;
		scope m_scope;
	};

	namespace_::namespace_(char const* name)
		: scope(luabind::unique_ptr<detail::registration>(
			m_registration = luabind_new<registration_>(name)))
	{
	}

	namespace_& namespace_::operator[](scope s)
	{
		m_registration->m_scope.operator,(s);
		return *this;
	}

} // namespace luabind

