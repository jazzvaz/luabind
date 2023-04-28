// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/lua_include.hpp>
#include <luabind/typeid.hpp>
#include <luabind/detail/meta.hpp>

namespace luabind::adl
{
    class object;
    class argument;
    template <class Base>
    struct table;
} // namespace luabind::adl

namespace luabind
{
    using adl::object;
    using adl::argument;
    using adl::table;
} // namespace luabind

namespace luabind::detail
{
    LUABIND_API luabind::string get_class_name(lua_State* L, type_id const& i);

    template <class T, class Enable = void>
    struct type_to_string
    {
        static void get(lua_State* L)
        {
            luabind::string const name = get_class_name(L, typeid(T));
            lua_pushlstring(L, name.c_str(), name.size());
        }
    };

    template <class T>
    struct type_to_string<T*>
    {
        static void get(lua_State* L)
        {
            type_to_string<T>::get(L);
            lua_pushliteral(L, "*");
            lua_concat(L, 2);
        }
    };

    template <class T>
    struct type_to_string<T&>
    {
        static void get(lua_State* L)
        {
            type_to_string<T>::get(L);
            lua_pushliteral(L, "&");
            lua_concat(L, 2);
        }
    };

    template <class T>
    struct type_to_string<T const>
    {
        static void get(lua_State* L)
        {
            type_to_string<T>::get(L);
            lua_pushliteral(L, " const");
            lua_concat(L, 2);
        }
    };

#define LUABIND_TYPE_TO_STRING(x) \
    template <> \
    struct type_to_string<x> \
    { \
        static void get(lua_State* L) { lua_pushliteral(L, #x); } \
    };

#define LUABIND_INTEGRAL_TYPE_TO_STRING(x) \
    LUABIND_TYPE_TO_STRING(x) \
    LUABIND_TYPE_TO_STRING(unsigned x)

    LUABIND_INTEGRAL_TYPE_TO_STRING(char)
    LUABIND_INTEGRAL_TYPE_TO_STRING(short)
    LUABIND_INTEGRAL_TYPE_TO_STRING(int)
    LUABIND_INTEGRAL_TYPE_TO_STRING(long)
    LUABIND_TYPE_TO_STRING(void)
    LUABIND_TYPE_TO_STRING(bool)
    LUABIND_TYPE_TO_STRING(luabind::string)
    LUABIND_TYPE_TO_STRING(lua_State)
    LUABIND_TYPE_TO_STRING(luabind::object)
    LUABIND_TYPE_TO_STRING(luabind::argument)

#undef LUABIND_INTEGRAL_TYPE_TO_STRING
#undef LUABIND_TYPE_TO_STRING

    template <class Base>
    struct type_to_string<table<Base>>
    {
        static void get(lua_State* L)
        {
            lua_pushliteral(L, "table");
        }
    };

    inline void format_signature_aux(lua_State*, bool, meta::type_list<>)
    {}

    template <class Signature>
    void format_signature_aux(lua_State* L, bool first, Signature)
    {
        if (!first)
            lua_pushliteral(L, ",");
        type_to_string<meta::front_t<Signature>>::get(L);
        format_signature_aux(L, false, meta::pop_front_t<Signature>());
    }

    template <class Signature>
    int format_signature(lua_State* L, char const* function, Signature, bool concat = true)
    {
        using first = meta::front_t<Signature>;
        type_to_string<first>::get(L);
        lua_pushliteral(L, " ");
        lua_pushstring(L, function);
        lua_pushliteral(L, "(");
        format_signature_aux(L, true, meta::pop_front_t<Signature>());
        lua_pushliteral(L, ")");
        size_t ncat = meta::size_v<Signature> *2 + 2 + (meta::size<Signature>::value == 1 ? 1 : 0);
        if (concat)
        {
            lua_concat(L, static_cast<int>(ncat));
            ncat = 1;
        }
        return static_cast<int>(ncat);
    }
} // namespace luabind::detail
