// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <luabind/detail/policy.hpp>

namespace luabind::detail
{
	struct copy_converter
	{
		template <class T>
		void to_lua(lua_State* L, T const& x)
		{ value_converter().to_lua(L, x); }

		template <class T>
		void to_lua(lua_State* L, T* x)
		{
			if (!x)
				lua_pushnil(L);
			else
				to_lua(L, *x);
		}
	};

	struct copy_policy
	{
		template <class T, class Direction>
		struct specialize
		{
			static_assert(std::is_same_v<Direction, cpp_to_lua>, "Copy policy only supports cpp -> lua");
			using type = copy_converter;
		};
	};
} // namespace luabind::detail

namespace luabind::policy
{
	template <uint32_t N>
	using copy = converter_injector<N, detail::copy_policy>;
} // namespace luabind::policy
