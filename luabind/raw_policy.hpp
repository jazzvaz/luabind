// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind {
	namespace detail {

		struct raw_converter
		{
			enum { consumed_args = 0 };

			lua_State* to_cpp(lua_State* L, by_pointer<lua_State>, int)
			{
				return L;
			}

			static int match(...)
			{
				return 0;
			}

			void converter_postcall(lua_State*, by_pointer<lua_State>, int) {}
		};

		struct raw_policy
		{
			template<class T, class Direction>
			struct specialize
			{
				using type = raw_converter;
			};
		};

	}
	
	namespace policy
	{
		template<unsigned int N>
		using raw = converter_policy_injector<N, detail::raw_policy>;
	}
} // namespace luabind
