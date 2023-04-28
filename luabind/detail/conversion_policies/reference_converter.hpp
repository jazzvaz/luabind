// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <type_traits>
#include <luabind/detail/conversion_policies/conversion_base.hpp>
#include <luabind/detail/conversion_policies/pointer_converter.hpp>
#include <luabind/back_reference.hpp>

namespace luabind::detail
{
    struct ref_converter : pointer_converter
    {
        using type = ref_converter;
        using is_native = std::false_type;

        static constexpr int consumed_args = 1;

        template <class T>
        void to_lua(lua_State* L, T& ref)
        {
            if (luabind::get_back_reference(L, ref))
                return;
            make_pointee_instance(L, ref, std::false_type());
        }

        template <class T>
        T& to_cpp(lua_State* L, by_reference<T>, int index)
        {
            assert(!lua_isnil(L, index));
            return *pointer_converter::to_cpp(L, by_pointer<T>(), index);
        }

        template <class T>
        T const& to_cpp(lua_State* L, by_const_reference<T>, int index)
        {
            assert(!lua_isnil(L, index));
            return *pointer_converter::to_cpp(L, by_const_pointer<T>(), index);
        }

        template <class T>
        int match(lua_State* L, by_reference<T>, int index)
        {
            return pointer_converter::match(L, by_pointer<T>(), index);
        }

        template <class T>
        int match(lua_State* L, by_const_reference<T>, int index)
        {
            return pointer_converter::match(L, by_const_pointer<T>(), index);
        }

        template <class T>
        void converter_postcall(lua_State*, T, int /*index*/) {}
    };
} // namespace luabind::detail
