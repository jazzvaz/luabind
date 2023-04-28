// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#if defined(DEBUG) && defined(NDEBUG)
static_assert(false, "Do not define NDEBUG macro in DEBUG configuration");
#endif

// the maximum number of arguments of functions that's
// registered. Must at least be 2
#ifndef LUABIND_MAX_ARITY
# define LUABIND_MAX_ARITY 100
#elif LUABIND_MAX_ARITY <= 1
# undef LUABIND_MAX_ARITY
# define LUABIND_MAX_ARITY 2
#endif

// the maximum number of classes one class
// can derive from
// max bases must at least be 1
#ifndef LUABIND_MAX_BASES
# define LUABIND_MAX_BASES 100
#elif LUABIND_MAX_BASES <= 0
# undef LUABIND_MAX_BASES
# define LUABIND_MAX_BASES 1
#endif

// LUABIND_NO_ERROR_CHECKING
// define this to remove all error checks
// this will improve performance and memory
// footprint.
// if it is defined matchers will only be called on
// overloaded functions, functions that's
// not overloaded will be called directly. The
// parameters on the lua stack are assumed
// to match those of the function.
// exceptions will still be catched when there's
// no error checking.

// LUABIND_NOT_THREADSAFE
// this define will make luabind non-thread safe. That is,
// it will rely on a static variable. You can still have
// multiple lua states and use coroutines, but only
// one of your real threads may run lua code.

// LUABIND_NO_EXCEPTIONS
// this define will disable all usage of try, catch and throw in
// luabind. This will in many cases disable runtime-errors, such
// as invalid casts, when calling lua-functions that fails or
// returns values that cannot be converted by the given policy.
// Luabind requires that no function called directly or indirectly
// by luabind throws an exception (throwing exceptions through
// C code has undefined behavior, lua is written in C).

// LUABIND_CUSTOM_ALLOCATOR
// this define will make luabind use user provided luabind::allocator
// instead of global new/delete operators. Keep in mind that Lua garbage collector
// will use this custom allocator to deallocate adopted C++ objects, unless
// they are referenced by a smart pointer with a custom deleter.

#if defined (_WIN32)
# ifdef LUABIND_BUILDING
#  define LUABIND_API __declspec(dllexport)
# else
#  define LUABIND_API __declspec(dllimport)
# endif
#elif defined (__CYGWIN__)
# ifdef LUABIND_BUILDING
#  define LUABIND_API __attribute__ ((dllexport))
# else
#  define LUABIND_API __attribute__ ((dllimport))
# endif
#elif defined(__GNUC__)
# define LUABIND_API __attribute__ ((visibility("default")))
#else
# error "Unsupported compiler"
#endif

#ifndef _WIN32
# include <cstddef>
# define __cdecl
#endif

namespace luabind
{
    LUABIND_API void disable_super_deprecation();
    LUABIND_API void set_custom_type_marking(bool enable);
    LUABIND_API bool is_nil_conversion_allowed();
    LUABIND_API void allow_nil_conversion(bool allow);

    namespace detail
    {
        constexpr int max_argument_count = 100;
        constexpr int max_hierarchy_depth = 100;
    }

    constexpr int no_match = -(detail::max_argument_count * detail::max_hierarchy_depth + 1);
} // namespace luabind

#include <luabind/types.hpp>
