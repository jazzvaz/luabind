// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#define LUABIND_BUILDING

#include <luabind/config.hpp>

#ifdef LUABIND_CUSTOM_ALLOCATOR
#include <luabind/memory.hpp>

namespace luabind
{
	LUABIND_API allocator_func allocator = nullptr;
	LUABIND_API void* allocator_context = nullptr;
}
#endif
