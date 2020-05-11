// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2006 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

namespace luabind
{
	namespace detail
	{
		struct nil_type {};
	}

	constexpr detail::nil_type nil;
}
