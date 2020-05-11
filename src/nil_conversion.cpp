// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#define LUABIND_BUILDING

#include <luabind/config.hpp>

namespace luabind
{
	static bool nil_conversion_allowed = false;

	LUABIND_API bool is_nil_conversion_allowed()
	{
		return nil_conversion_allowed;
	}

	LUABIND_API void allow_nil_conversion(bool allowed)
	{
		nil_conversion_allowed = allowed;
	}
} // namespace luabind
