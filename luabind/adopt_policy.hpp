// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/wrapper_base.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/back_reference_fwd.hpp>

namespace luabind::detail
{
    template <class T>
    void adjust_backref_ownership(T* ptr)
    {
        if constexpr (std::is_polymorphic_v<T>)
        {
            auto p = dynamic_cast<wrap_base*>(ptr);
            if (!p)
                return;
            wrapped_self_t& wrapper = wrap_access::ref(*p);
            wrapper.get(wrapper.state());
            wrapper.m_strong_ref.pop(wrapper.state());
        }
    }

    template <class Pointer, class Direction = lua_to_cpp>
    struct adopt_pointer : pointer_converter
    {
        using type = adopt_pointer;

        static constexpr int consumed_args = 1;

        template <class T>
        T* to_cpp(lua_State* L, by_pointer<T>, int index)
        {
            T* ptr = pointer_converter::to_cpp(L, decorate_type_t<T*>(), index);
            auto* obj = static_cast<object_rep*>(lua_touserdata(L, index));
            if (obj)
                obj->release();
            adjust_backref_ownership(ptr);
            return ptr;
        }

        template <class T>
        int match(lua_State* L, by_pointer<T>, int index)
        {
            return pointer_converter::match(L, decorate_type_t<T*>(), index);
        }

        template <class T>
        void converter_postcall(lua_State*, T, int) {}
    };

    template <class Pointer, class T>
    struct pointer_or_default
    {
        using type = Pointer;
    };

    template <class T>
    struct pointer_or_default<void, T>
    {
        using type = luabind::unique_ptr<T>;
    };

    template <class Pointer>
    struct adopt_pointer<Pointer, cpp_to_lua>
    {
        using type = adopt_pointer;

        template <class T>
        void to_lua(lua_State* L, T* ptr)
        {
            if (!ptr)
            {
                lua_pushnil(L);
                return;
            }
            // if there is a back_reference, then the
            // ownership will be removed from the
            // back reference and put on the lua stack.
            if (luabind::move_back_reference(L, ptr))
                return;
            using pointer_type = typename pointer_or_default<Pointer, T>::type;
            make_pointer_instance(L, pointer_type(ptr));
        }
    };

    template <class Pointer>
    struct adopt_policy_impl
    {
        template <class T, class Direction>
        struct specialize
        {
            static_assert(detail::is_nonconst_pointer_v<T>, "Adopt policy only accepts non-const pointers");
            using type = adopt_pointer<Pointer, Direction>;
        };
    };
} // namespace luabind::detail

namespace luabind::policy
{
    template <uint32_t N, typename Pointer = void>
    using adopt = converter_injector<N, detail::adopt_policy_impl<Pointer>>;
} // namespace luabind::policy
