// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <type_traits>
#include <luabind/lua_include.hpp>
#include <luabind/back_reference.hpp>
#include <luabind/detail/object_rep.hpp>

namespace luabind::detail
{
    struct value_converter
    {
        using type = value_converter;
        using is_native = std::false_type;

        static constexpr int consumed_args = 1;

        void* result = nullptr;

        template <class T>
        void to_lua(lua_State* L, T&& x)
        {
            if (luabind::get_back_reference(L, x))
                return;
            make_value_instance(L, std::forward<T>(x));
        }

        template <class T>
        T to_cpp(lua_State*, by_value<T>, int)
        {
            return *static_cast<T*>(result);
        }

        template <class T>
        int match(lua_State* L, by_value<T>, int index)
        {
            // special case if we get nil in, try to match the holder type
            if (lua_isnil(L, index))
                return no_match;
            object_rep* obj = get_instance(L, index);
            if (!obj)
                return no_match;
            auto [ptr, score] = obj->get_instance(registered_class<T>::id);
            result = ptr;
            return score;
        }

        template <class T>
        void converter_postcall(lua_State*, T, int) {}
    };
} // namespace luabind::detail
