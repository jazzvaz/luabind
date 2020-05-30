// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <luabind/lua_include.hpp>
#include <luabind/lua_proxy.hpp>
#include <utility>
#include <cassert>

namespace luabind
{
    // A reference to a Lua value. Represents an entry in the registry table.
    class handle
    {
    public:
        handle() = default;
        handle(lua_State* interpreter, int stack_index);
        handle(lua_State* main, lua_State* interpreter, int stack_index);
        handle(handle const& other);
        ~handle();

        handle& operator=(handle const& other);
        void swap(handle& other);
        void push(lua_State* interpreter) const;
        lua_State* interpreter() const;
        bool is_valid() const;
        void replace(lua_State* interpreter, int stack_index);
        void pop(lua_State* L);
        void reset();

    private:
        lua_State* m_interpreter = nullptr;
        int m_index = LUA_NOREF;
    };

    inline handle::handle(handle const& other) :
        m_interpreter(other.m_interpreter)
    {
        if (!m_interpreter)
            return;
        other.push(m_interpreter);
        m_index = luaL_ref(m_interpreter, LUA_REGISTRYINDEX);
    }

    inline handle::handle(lua_State* interpreter, int stack_index) :
        m_interpreter(interpreter)
    {
        lua_pushvalue(interpreter, stack_index);
        m_index = luaL_ref(interpreter, LUA_REGISTRYINDEX);
    }

    inline handle::handle(lua_State* main, lua_State* interpreter, int stack_index) :
        m_interpreter(main)
    {
        lua_pushvalue(interpreter, stack_index);
        m_index = luaL_ref(interpreter, LUA_REGISTRYINDEX);
    }

    inline handle::~handle()
    {
        if (is_valid())
            luaL_unref(m_interpreter, LUA_REGISTRYINDEX, m_index);
    }

    inline handle& handle::operator=(handle const& other)
    {
        handle(other).swap(*this);
        return *this;
    }

    inline void handle::swap(handle& other)
    {
        std::swap(m_interpreter, other.m_interpreter);
        std::swap(m_index, other.m_index);
    }

    inline void handle::push(lua_State* interpreter) const
    { lua_rawgeti(interpreter, LUA_REGISTRYINDEX, m_index); }

    inline lua_State* handle::interpreter() const
    { return m_interpreter; }

    inline bool handle::is_valid() const
    {
        if (m_index == LUA_NOREF)
        {
            assert(!m_interpreter);
            return false;
        }
        else
        {
            assert(m_interpreter);
            return true;
        }
    }

    inline void handle::replace(lua_State* interpreter, int stack_index)
    {
        lua_pushvalue(interpreter, stack_index);
        lua_rawseti(interpreter, LUA_REGISTRYINDEX, m_index);
    }

    inline void handle::reset()
    {
        if (is_valid())
        {
            luaL_unref(m_interpreter, LUA_REGISTRYINDEX, m_index);
            m_index = LUA_NOREF;
        }
    }

    inline void handle::pop(lua_State* L)
    {
        reset();
        m_interpreter = L;
        m_index = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    template <>
    struct lua_proxy_traits<handle>
    {
        using is_specialized = std::true_type;

        static lua_State* interpreter(handle const& value)
        { return value.interpreter(); }

        static void unwrap(lua_State* interpreter, handle const& value)
        { value.push(interpreter); }

        static bool check(...)
        { return true; }
    };
} // namespace luabind
