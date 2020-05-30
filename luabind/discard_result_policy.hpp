// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>    // for index_map, etc
#include <luabind/detail/primitives.hpp>  // for null_type, etc
#include <luabind/lua_include.hpp>

namespace luabind::detail
{
    struct discard_converter
    {
        template <class T>
        void to_lua(lua_State*, T) {}
    };

    struct discard_result_policy
    {
        template <class T, class Direction>
        struct specialize
        {
            static_assert(std::is_same_v<Direction, cpp_to_lua>, "Can only convert from cpp to lua");
            using type = discard_converter;
        };
    };
} // namespace luabind::detail

namespace luabind::policy
{
    using discard_result = converter_injector<0, detail::discard_result_policy>;
} // namespace luabind::policy
