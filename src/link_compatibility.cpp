// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include <luabind/detail/link_compatibility.hpp>

namespace luabind::detail
{
#ifdef LUABIND_NOT_THREADSAFE
        void not_threadsafe_defined_conflict() {}
#else
        void not_threadsafe_not_defined_conflict() {}
#endif

#ifdef LUABIND_NO_ERROR_CHECKING
        void no_error_checking_defined_conflict() {}
#else
        void no_error_checking_not_defined_conflict() {}
#endif
}
