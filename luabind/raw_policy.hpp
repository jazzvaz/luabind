// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/lua_include.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind::detail
{
	struct raw_converter
	{
		static constexpr int consumed_args = 0;

		inline lua_State* to_cpp(lua_State* L, by_pointer<lua_State>, int)
		{ return L; }

		inline static int match(...)
		{ return 0; }

		inline void converter_postcall(lua_State*, by_pointer<lua_State>, int) {}
	};

	struct raw_policy
	{
		template <class T, class Direction>
		struct specialize
		{
			using type = raw_converter;
		};
	};
} // namespace luabind::detail

namespace luabind::policy
{
	template <uint32_t N>
	using raw = converter_injector<N, detail::raw_policy>;
} // namespace luabind::policy
