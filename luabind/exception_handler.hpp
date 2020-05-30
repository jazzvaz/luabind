// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <luabind/config.hpp>           // for LUABIND_API
#include <type_traits>
#include <luabind/lua_include.hpp>
#include <luabind/detail/meta.hpp>

#ifndef LUABIND_NO_EXCEPTIONS
namespace luabind::detail
{
    struct LUABIND_API exception_handler_base
    {
        virtual ~exception_handler_base() = default;
        virtual void handle(lua_State*) const = 0;
        void try_next(lua_State*) const;

        exception_handler_base* next = nullptr;
    };

    template <class E, class Handler>
    struct exception_handler : exception_handler_base
    {
        using argument = E const&;

        exception_handler(Handler handler) :
            handler(handler)
        {}

        void handle(lua_State* L) const
        {
            try
            {
                try_next(L);
            }
            catch (argument e)
            {
                handler(L, e);
            }
        }

        Handler handler;
    };

    LUABIND_API void handle_exception_aux(lua_State* L);
    LUABIND_API void register_exception_handler(exception_handler_base*);
} // namespace luabind::detail
#endif

namespace luabind
{
    template <class E, class Handler>
    void register_exception_handler(Handler handler, meta::type<E>* = nullptr)
    {
#ifndef LUABIND_NO_EXCEPTIONS
        auto p = luabind_new<detail::exception_handler<E, Handler>>(handler);
        detail::register_exception_handler(p);
#endif
    }
} // namespace luabind
