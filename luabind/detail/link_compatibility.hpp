// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

namespace luabind::detail
{
#ifdef LUABIND_NOT_THREADSAFE
    LUABIND_API void not_threadsafe_defined_conflict();
#else
    LUABIND_API void not_threadsafe_not_defined_conflict();
#endif

#ifdef LUABIND_NO_ERROR_CHECKING
    LUABIND_API void no_error_checking_defined_conflict();
#else
    LUABIND_API void no_error_checking_not_defined_conflict();
#endif

    inline void check_link_compatibility()
    {
#ifdef DEBUG
# ifdef LUABIND_NOT_THREADSAFE
        not_threadsafe_defined_conflict();
# else
        not_threadsafe_not_defined_conflict();
# endif
# ifdef LUABIND_NO_ERROR_CHECKING
        no_error_checking_defined_conflict();
# else
        no_error_checking_not_defined_conflict();
# endif
#endif
    }
} // namespace luabind::detail
