// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#ifndef LUABIND_DISCARD_RESULT_POLICY_HPP_INCLUDED
#define LUABIND_DISCARD_RESULT_POLICY_HPP_INCLUDED

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>    // for index_map, etc
#include <luabind/detail/primitives.hpp>  // for null_type, etc
#include <luabind/lua_include.hpp>

namespace luabind {
	namespace detail {

		struct discard_converter
		{
			template<class T>
			void to_lua(lua_State*, T) {}
		};

		struct discard_result_policy
		{
			struct can_only_convert_from_cpp_to_lua {};

			template<class T, class Direction>
			struct specialize
			{
				static_assert(std::is_same< Direction, cpp_to_lua >::value, "Can only convert from cpp to lua");
				using type = discard_converter;
			};
		};

	}
}

namespace luabind
{
	namespace policy
	{
		using discard_result = converter_policy_injector<0, detail::discard_result_policy>;
	}
}

#endif // LUABIND_DISCARD_RESULT_POLICY_HPP_INCLUDED

