// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#include <luabind/function.hpp>
#include <luabind/detail/object.hpp>
#include <luabind/make_function.hpp>
#include <luabind/detail/conversion_policies/conversion_policies.hpp>
#include <luabind/detail/object.hpp>
#include <luabind/lua_extensions.hpp>

namespace luabind::detail
{
    // A pointer to this is used as a tag value to identify functions exported by luabind.
    static char function_tag;
    // same, but for non-default functions (not from m_default_members)
    static char function_tag_ndef;

    static int function_destroy(lua_State* L)
    {
        function_object* fn = *(function_object**)lua_touserdata(L, 1);
        luabind_delete(fn);
        return 0;
    }

    static void push_function_metatable(lua_State* L)
    {
        lua_rawgetp(L, LUA_REGISTRYINDEX, &function_tag);
        if (lua_istable(L, -1))
            return;
        lua_pop(L, 1);
        lua_createtable(L, 0, 1); // One non-sequence entry for __gc.
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, &function_destroy);
        lua_rawset(L, -3);
        lua_pushvalue(L, -1);
        lua_rawsetp(L, LUA_REGISTRYINDEX, &function_tag);
    }

    bool is_luabind_function(lua_State* L, int index, bool allow_default /*= true*/)
    {
        if (!lua_getupvalue(L, index, 2))
            return false;
        void* tag = lua_touserdata(L, -1);
        bool result = tag == &function_tag && allow_default || tag == &function_tag_ndef;
        lua_pop(L, 1);
        return result;
    }

    static bool is_luabind_function(object const& obj)
    {
        obj.push(obj.interpreter());
        bool result = is_luabind_function(obj.interpreter(), -1);
        lua_pop(obj.interpreter(), 1);
        return result;
    }

    void add_overload(object const& context, char const* name, object const& fn)
    {
        function_object* f = *touserdata<function_object*>(std::get<1>(getupvalue(fn, 1)));
        f->name = name;
        if (object overloads = context[name])
        {
            if (is_luabind_function(overloads) && is_luabind_function(fn))
            {
                f->next = *touserdata<function_object*>(std::get<1>(getupvalue(overloads, 1)));
                f->keepalive = overloads;
            }
        }
        context[name] = fn;
    }

    object make_function_aux(lua_State* L, function_object* impl, bool default_scope /*= false*/)
    {
        void* storage = lua_newuserdata(L, sizeof(function_object*));
        push_function_metatable(L);
        *(function_object**)storage = impl;
        lua_setmetatable(L, -2);
        void* tag = default_scope ? &function_tag : &function_tag_ndef;
        lua_pushlightuserdata(L, tag);
        lua_pushcclosure(L, impl->entry, 2);
        stack_pop pop(L, 1);
        return object(from_stack(L, -1));
    }

    void invoke_context::format_error(lua_State* L, function_object const* overloads) const
    {
        char const* function_name =
            overloads->name.empty() ? "<unknown>" : overloads->name.c_str();
        int stacksize = lua_gettop(L);
        int cs = lua_gettop(L);
        if (candidate_index == 0)
        {
            lua_pushliteral(L, "No matching overload found, candidates:\n");
            int count = 0;
            for (function_object const* f = overloads; f; f = f->next)
            {
                if (count)
                    lua_pushliteral(L, "\n");
                f->format_signature(L, function_name);
                count++;
            }
        }
        else
        {
            lua_pushliteral(L, "Ambiguous, candidates:\n");
            for (int i = 0; i < candidate_index; i++)
            {
                if (i)
                    lua_pushliteral(L, "\n");
                candidates[i]->format_signature(L, function_name);
            }
        }
        if (extra_candidates)
        {
            assert(candidate_index == max_candidates);
            lua_pushfstring(L, "\nand %d additional overload(s) not shown", extra_candidates);
            lua_concat(L, 2);
        }
        lua_pushfstring(L, "\nPassed arguments [%d]: ", stacksize);
        if (!stacksize)
            lua_pushliteral(L, "<no arguments>\n");
        else
        {
            for (int _index = 1; _index <= stacksize; _index++)
            {
                if (_index > 1)
                    lua_pushliteral(L, ", ");
                // Arg Type
                lua_pushstring(L, lua_typename(L, lua_type(L, _index)));
                // Arg Value
                lua_pushliteral(L, " (");
                luaL_tolstring_diag(L, _index, nullptr);
                lua_pushliteral(L, ")");
            }
            lua_pushliteral(L, "\n");
        }
        lua_concat(L, lua_gettop(L) - stacksize);
    }
} // namespace luabind::detail
