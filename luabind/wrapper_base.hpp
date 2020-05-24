// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/weak_ref.hpp>
#include <luabind/handle.hpp>
#include <luabind/detail/meta.hpp>
#include <luabind/error.hpp>
#include <type_traits>

namespace luabind
{
	namespace detail
	{
		struct wrap_access;

		// implements the selection between dynamic dispatch
		// or default implementation calls from within a virtual
		// function wrapper. The input is the self reference on
		// the top of the stack. Output is the function to call
		// on the top of the stack (the input self reference will
		// be popped)
		LUABIND_API void do_call_member_selection(lua_State* L, char const* name);

		template <class R, typename PolicyList = meta::type_list<>, uint32_t... Indices, typename... Args>
		R call_member_impl(lua_State* L, meta::index_list<Indices...>, Args&&... args);
	} // namespace detail

	struct wrapped_self_t : weak_ref
	{
		handle m_strong_ref;
	};

	struct wrap_base
	{
		friend struct detail::wrap_access;

		template <class R, typename... Args>
		R call(char const* name, Args&&... args) const
		{
			// this will be cleaned up by the proxy object
			// once the call has been made

			// TODO: what happens if this virtual function is
			// dispatched from a lua thread where the state
			// pointer is different?

			// get the function
			lua_State* L = m_self.state();
			m_self.get(L);
			assert(!lua_isnil(L, -1));
			detail::do_call_member_selection(L, name);
			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);
				throw unresolved_name("Attempt to call nonexistent function", name);
			}
			// push the self reference as the first parameter
			m_self.get(L);
			// now the function and self objects
			// are on the stack. These will both
			// be popped by pcall
			using index_list = meta::index_range<1, sizeof...(Args)+1>;
			return detail::call_member_impl<R>(L, index_list(), std::forward<Args>(args)...);
		}

	private:
		wrapped_self_t m_self;
	};
} // namespace luabind

namespace luabind::detail
{
	struct wrap_access
	{
		static wrapped_self_t const& ref(wrap_base const& b)
		{ return b.m_self; }

		static wrapped_self_t& ref(wrap_base& b)
		{ return b.m_self; }
	};
} // namespace luabind::detail
