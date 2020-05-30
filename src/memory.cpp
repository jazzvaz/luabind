// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include <luabind/config.hpp>

#ifdef LUABIND_CUSTOM_ALLOCATOR
#include <luabind/memory.hpp>

namespace luabind
{
    allocator_func allocator = nullptr;
    void* allocator_context = nullptr;
} // namespace luabind
#endif
