// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <type_traits>
#include <luabind/lua_include.hpp>
#include <luabind/detail/make_instance.hpp>
#include <luabind/back_reference.hpp>

namespace luabind::detail
{
    struct pointer_converter
    {
        using type = pointer_converter;
        using is_native = std::false_type;

        static constexpr int consumed_args = 1;

        void* result = nullptr;

        template <class T>
        void to_lua(lua_State* L, T* ptr)
        {
            if (!ptr)
            {
                lua_pushnil(L);
                return;
            }
            if (luabind::get_back_reference(L, ptr))
                return;
            make_pointer_instance(L, ptr);
        }

        template <class T>
        T* to_cpp(lua_State*, by_pointer<T>, int /*index*/)
        { return static_cast<T*>(result); }

        template <class T>
        T const* to_cpp(lua_State*, by_const_pointer<T>, int /*index*/)
        { return static_cast<T const*>(result); }

        template <class T, template <class> typename Decorator>
        int match(lua_State* L, Decorator<T>, int index)
        {
            static_assert(std::is_same_v<Decorator<T>, by_pointer<T>>
                || std::is_same_v<Decorator<T>, by_const_pointer<T>>,
                "Only by_pointer<T> or by_const_pointer<T> is accepted");
            if (lua_isnil(L, index))
                return 0;
            object_rep* obj = get_instance(L, index);
            // if the type is not one of our own registered types, classify it as a non-match
            if (!obj)
                return no_match;
            if constexpr (std::is_same_v<Decorator<T>, by_pointer<T>>)
            {
                if (obj->is_const())
                    return no_match;
            }
            auto [ptr, score] = obj->get_instance(registered_class<T>::id);
            if constexpr (std::is_same_v<Decorator<T>, by_const_pointer<T>>)
            {
                if (score >= 0 && !obj->is_const())
                    score += 10;
            }
            result = ptr;
            return score;
        }

        template <class T>
        void converter_postcall(lua_State*, T, int /*index*/) {}
    };
} // namespace luabind::detail
