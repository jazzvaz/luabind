// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#ifndef LUABIND_YIELD_POLICY_HPP_INCLUDED
#define LUABIND_YIELD_POLICY_HPP_INCLUDED

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind {

	namespace detail {

		struct yield_policy
		{
			static void postcall(lua_State*, int /*results*/, meta::index_list_tag) {}
		};

	}

	namespace policy
	{
		using yield = call_policy_injector<detail::yield_policy>;
	}
}

#endif // LUABIND_YIELD_POLICY_HPP_INCLUDED

