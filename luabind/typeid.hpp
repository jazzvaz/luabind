// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <typeindex>
#include <luabind/config.hpp>
#include <luabind/detail/type_traits.hpp>

#pragma warning(push)
#pragma warning(disable : 4275)

namespace luabind
{
    class LUABIND_API type_id : public std::type_index
    {
    public:
        type_id() :
            std::type_index(typeid(null_type))
        {}

        type_id(std::type_info const& id) :
            std::type_index(id)
        {}
    };
} // namespace luabind

#pragma warning(pop) 