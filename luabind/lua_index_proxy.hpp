// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <cassert>
#include <luabind/lua_proxy_interface.hpp>
#include <luabind/detail/stack_utils.hpp>
#include <luabind/nil.hpp>

namespace luabind::adl
{
    class object;

    template <class Next>
    class index_proxy : public lua_proxy_interface<index_proxy<Next>>
    {
    public:
        using this_type = index_proxy<Next>;

        template <class Key>
        index_proxy(Next const& next, lua_State* interpreter, Key const& key) :
            m_interpreter(interpreter),
            m_key_index(lua_gettop(interpreter) + 1),
            m_next(next)
        {
            lua_stack::push(m_interpreter, key);
        }

        index_proxy(index_proxy const& other) :
            m_interpreter(other.m_interpreter),
            m_key_index(other.m_key_index),
            m_next(other.m_next)
        {
            other.m_interpreter = nullptr;
        }

        ~index_proxy()
        {
            if (m_interpreter)
                lua_pop(m_interpreter, 1);
        }

        // This is non-const to prevent conversion on lvalues.
        // defined in luabind/detail/object.hpp
        operator object();

        // this will set the value to nil
        this_type& operator=(luabind::detail::nil_type)
        {
            lua_proxy_traits<Next>::unwrap(m_interpreter, m_next);
            detail::stack_pop pop(m_interpreter, 1);
            lua_pushvalue(m_interpreter, m_key_index);
            lua_pushnil(m_interpreter);
            lua_settable(m_interpreter, -3);
            return *this;
        }

        template <class T>
        this_type& operator=(T&& value)
        {
            lua_proxy_traits<Next>::unwrap(m_interpreter, m_next);
            detail::stack_pop pop(m_interpreter, 1);
            lua_pushvalue(m_interpreter, m_key_index);
            lua_stack::push(m_interpreter, std::forward<T>(value));
            lua_settable(m_interpreter, -3);
            return *this;
        }

        this_type& operator=(this_type const& value)
        {
            lua_proxy_traits<Next>::unwrap(m_interpreter, m_next);
            detail::stack_pop pop(m_interpreter, 1);
            lua_pushvalue(m_interpreter, m_key_index);
            lua_stack::push(m_interpreter, value);
            lua_settable(m_interpreter, -3);
            return *this;
        }

        template <class T>
        index_proxy<this_type> operator[](T const& key)
        {
            return index_proxy<this_type>(*this, m_interpreter, key);
        }

        void push(lua_State* interpreter);

        lua_State* interpreter() const
        {
            return m_interpreter;
        }

    private:
        struct hidden_type {};
        mutable lua_State* m_interpreter;
        int m_key_index;
        Next const& m_next;
    };

    template <class Next>
    void index_proxy<Next>::push(lua_State* interpreter)
    {
        assert(interpreter == m_interpreter);
        lua_proxy_traits<Next>::unwrap(m_interpreter, m_next);
        lua_pushvalue(m_interpreter, m_key_index);
        lua_gettable(m_interpreter, -2);
        lua_remove(m_interpreter, -2);
    }
} // namespace luabind::adl

namespace luabind
{
    template <class T>
    struct lua_proxy_traits<adl::index_proxy<T>>
    {
        using is_specialized = std::true_type;

        template <class Next>
        static lua_State* interpreter(adl::index_proxy<Next> const& proxy)
        {
            return proxy.interpreter();
        }

        template <class Next>
        static void unwrap(lua_State* interpreter, adl::index_proxy<Next> const& proxy)
        {
            const_cast<adl::index_proxy<Next>&>(proxy).push(interpreter);
        }
    };
} // namespace luabind
