// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2006 The Luabind Authors

#ifndef LUABIND_NIL_HPP
#define LUABIND_NIL_HPP

#include <luabind/config.hpp>

namespace luabind
{
	namespace detail
	{
		struct nil_type {};
	}

	constexpr detail::nil_type nil;
}

#endif

