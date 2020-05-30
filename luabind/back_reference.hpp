// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/lua_state_fwd.hpp>
#include <type_traits>
#include <luabind/wrapper_base.hpp>
#include <luabind/pointer_traits.hpp>

namespace luabind
{
    struct wrap_base;
} // namespace luabind

namespace luabind::detail
{
    template <class T>
    wrap_base const* get_back_reference(T const& x)
    {
        if constexpr (has_get_pointer_v<T>)
        {
            auto p = get_pointer(x);
            if constexpr (std::is_polymorphic_v<decltype(p)>)
                return dynamic_cast<wrap_base const*>(p);
        }
        else
        {
            auto p = &x;
            if constexpr (std::is_polymorphic_v<decltype(p)>)
                return dynamic_cast<wrap_base const*>(p);
        }
        return nullptr;
    }
} // namespace luabind::detail

namespace luabind
{
    template <class T>
    bool get_back_reference(lua_State* L, T const& x)
    {
        if (wrap_base const* w = detail::get_back_reference(x))
        {
            detail::wrap_access::ref(*w).get(L);
            return true;
        }
        return false;
    }

    template <class T>
    bool move_back_reference(lua_State* L, T const& x)
    {
        if (wrap_base* w = const_cast<wrap_base*>(detail::get_back_reference(x)))
        {
            assert(detail::wrap_access::ref(*w).m_strong_ref.is_valid());
            detail::wrap_access::ref(*w).get(L);
            detail::wrap_access::ref(*w).m_strong_ref.reset();
            return true;
        }
        return false;
    }
} // namespace luabind
