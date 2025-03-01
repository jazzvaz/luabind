// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include <algorithm>
#include <luabind/lua_include.hpp>
#include <luabind/config.hpp>
#include <luabind/weak_ref.hpp>
#include <cassert>

namespace luabind
{
    static char weak_table_tag;
    static char impl_table_tag;

    static void get_weak_table(lua_State* L)
    {
        lua_rawgetp(L, LUA_REGISTRYINDEX, &weak_table_tag);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            lua_newtable(L);
            // metatable
            lua_createtable(L, 0, 1); // One non-sequence entry for __mode.
            lua_pushliteral(L, "__mode");
            lua_pushliteral(L, "v");
            lua_rawset(L, -3);
            // set metatable
            lua_setmetatable(L, -2);
            lua_pushvalue(L, -1);
            lua_rawsetp(L, LUA_REGISTRYINDEX, &weak_table_tag);
        }
    }

    static void get_impl_table(lua_State* L)
    {
        lua_pushlightuserdata(L, &impl_table_tag);
        lua_rawget(L, LUA_REGISTRYINDEX);
        if (lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_rawsetp(L, LUA_REGISTRYINDEX, &impl_table_tag);
        }
    }

    struct weak_ref::impl
    {
        impl(lua_State* main, lua_State* s, int index) :
            state(main)
        {
            get_impl_table(s);
            lua_pushlightuserdata(s, this);
            ref = luaL_ref(s, -2);
            lua_pop(s, 1);
            get_weak_table(s);
            lua_pushvalue(s, index);
            lua_rawseti(s, -2, ref);
            lua_pop(s, 1);
        }

        ~impl()
        {
            get_impl_table(state);
            luaL_unref(state, -1, ref);
            lua_pop(state, 1);
        }

        int count = 0;
        lua_State* state;
        int ref = 0;
    };

    weak_ref::weak_ref(lua_State* main, lua_State* L, int index) :
        m_impl(luabind_new<impl>(main, L, index))
    {
        m_impl->count = 1;
    }

    weak_ref::weak_ref(weak_ref const& other) :
        m_impl(other.m_impl)
    {
        if (m_impl)
            m_impl->count++;
    }

    weak_ref::~weak_ref()
    {
        if (m_impl && --m_impl->count == 0)
            luabind_delete(m_impl);
    }

    weak_ref& weak_ref::operator=(weak_ref const& other)
    {
        weak_ref(other).swap(*this);
        return *this;
    }

    void weak_ref::swap(weak_ref& other)
    {
        std::swap(m_impl, other.m_impl);
    }

    int weak_ref::id() const
    {
        assert(m_impl);
        return m_impl->ref;
    }

    // L may not be the same pointer as
    // was used when creating this reference
    // since it may be a thread that shares
    // the same globals table.
    void weak_ref::get(lua_State* L) const
    {
        assert(m_impl);
        assert(L);
        get_weak_table(L);
        lua_rawgeti(L, -1, m_impl->ref);
        lua_remove(L, -2);
    }

    lua_State* weak_ref::state() const
    {
        assert(m_impl);
        return m_impl->state;
    }
} // namespace luabind
