// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include <luabind/lua_include.hpp> // for lua_pushstring, lua_rawset, etc
#include <luabind/config.hpp>
#include <luabind/typeid.hpp> // for type_id
#include <luabind/detail/class_registry.hpp> // for class_registry
#include <luabind/detail/class_rep.hpp> // for class_rep
#include <luabind/detail/garbage_collector.hpp> // for garbage_collector
#include <luabind/detail/object_rep.hpp> // push_instance_metatable
#include <cassert> // for assert
#include <map> // for map, etc
#include <utility> // for pair

namespace luabind::detail
{
    char class_registry_tag;

    static int create_cpp_class_metatable(lua_State* L)
    {
        lua_createtable(L, 0, 7);
        // mark the table with our unique tag
        // that says that the user data that has this
        // metatable is a class_rep
        lua_pushboolean(L, true);
        lua_rawsetp(L, -2, &classrep_tag);
        lua_pushliteral(L, "__gc");
        lua_pushcclosure(L, &garbage_collector<class_rep>, 0);
        lua_rawset(L, -3);
        lua_pushliteral(L, "__call");
        lua_pushcclosure(L, &class_rep::constructor_dispatcher, 0);
        lua_rawset(L, -3);
        lua_pushliteral(L, "__index");
        lua_pushcclosure(L, &class_rep::static_class_gettable, 0);
        lua_rawset(L, -3);
        lua_pushliteral(L, "__newindex");
        lua_pushcclosure(L, &class_rep::lua_settable_dispatcher, 0);
        lua_rawset(L, -3);
        lua_pushliteral(L, "__tostring");
        lua_pushcclosure(L, &class_rep::tostring, 0);
        lua_rawset(L, -3);
        // Direct calls to metamethods cannot be allowed, because the
        // callee trusts the caller to pass arguments of the right type.
        lua_pushliteral(L, "__metatable");
        lua_pushboolean(L, true);
        lua_rawset(L, -3);
        return luaL_ref(L, LUA_REGISTRYINDEX);
    }

    static int create_lua_class_metatable(lua_State* L)
    {
        return create_cpp_class_metatable(L);
    }

    class class_rep;

    class_registry::class_registry(lua_State* L) :
        m_cpp_class_metatable(create_cpp_class_metatable(L)),
        m_lua_class_metatable(create_lua_class_metatable(L))
    {
        push_instance_metatable(L);
        m_instance_metatable = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    class_registry* class_registry::get_registry(lua_State* L)
    {
#ifdef LUABIND_NOT_THREADSAFE
        // if we don't have to be thread safe, we can keep a
        // chache of the class_registry pointer without the
        // need of a mutex
        static lua_State* cache_key = nullptr;
        static class_registry* registry_cache = nullptr;
        if (cache_key == L)
            return registry_cache;
#endif
        lua_rawgetp(L, LUA_REGISTRYINDEX, &class_registry_tag);
        auto* p = static_cast<class_registry*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
#ifdef LUABIND_NOT_THREADSAFE
        cache_key = L;
        registry_cache = p;
#endif
        return p;
    }

    void class_registry::add_class(type_id const& info, class_rep* crep)
    {
        // class is already registered
        assert((m_classes.find(info) == m_classes.end())
            && "you are trying to register the same class twice");
        m_classes[info] = crep;
    }

    class_rep* class_registry::find_class(type_id const& info) const
    {
        auto i = m_classes.find(info);
        if (i == m_classes.end())
            return nullptr; // the type is not registered
        return i->second;
    }
} // namespace luabind::detail
