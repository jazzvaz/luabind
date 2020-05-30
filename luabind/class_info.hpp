// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/prefix.hpp>
#include <luabind/lua_include.hpp>
#include <luabind/luabind.hpp>
#include <luabind/object.hpp>

namespace luabind
{
    struct LUABIND_API class_info
    {
        luabind::string name;
        object methods;
        object attributes;
    };

    LUABIND_API class_info get_class_info(argument const&);
    // returns a table of bound class names
    LUABIND_API object get_class_names(lua_State* L);
    LUABIND_API void bind_class_info(lua_State*);
} // namespace luabind
