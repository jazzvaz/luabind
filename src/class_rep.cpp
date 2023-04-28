// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include <luabind/lua_include.hpp>
#include <luabind/detail/stack_utils.hpp>
#include <luabind/luabind.hpp>
#include <luabind/detail/class_rep.hpp>
#include <luabind/exception_handler.hpp>
#include <luabind/get_main_thread.hpp>
#include <utility>

namespace luabind
{
    static bool super_deprecation_disabled = false;

    void disable_super_deprecation()
    {
        super_deprecation_disabled = true;
    }
} // namespace luabind

namespace luabind::detail
{
    int property_tag(lua_State* L)
    {
        lua_pushliteral(L, "luabind: property_tag function can't be called");
        lua_error(L);
        return 0;
    }

    char classrep_tag;

    class_rep::class_rep(type_id const& type, const char* name, lua_State* L) :
        m_type(type),
        m_name(name),
        m_class_type(cpp_class),
        m_operator_cache(0)
    {
        shared_init(L);
    }

    void class_rep::shared_init(lua_State* L)
    {
        lua_newtable(L);
        handle(L, -1).swap(m_table);
        lua_newtable(L);
        handle(L, -1).swap(m_default_table);
        lua_pop(L, 2);
        class_registry* r = class_registry::get_registry(L);
        // the following line should be equivalent whether or not this is a cpp class
        assert((r->cpp_class() != LUA_NOREF) && "you must call luabind::open()");
        lua_rawgeti(L, LUA_REGISTRYINDEX, (m_class_type == cpp_class) ? r->cpp_class() : r->lua_class());
        lua_setmetatable(L, -2);
        handle(L, -1).swap(m_self_ref);
        m_instance_metatable = (m_class_type == cpp_class) ? r->cpp_instance() : r->lua_instance();
        lua_rawgetp(L, LUA_REGISTRYINDEX, &cast_graph_tag);
        m_casts = static_cast<cast_graph*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        lua_rawgetp(L, LUA_REGISTRYINDEX, &class_id_map_tag);
        m_classes = static_cast<class_id_map*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
    }

    class_rep::class_rep(lua_State* L, const char* name) :
        m_type(typeid(null_type)),
        m_name(name),
        m_class_type(lua_class),
        m_operator_cache(0)
    {
        shared_init(L);
    }

    // leaves object on lua stack
    std::pair<void*, void*> class_rep::allocate(lua_State* L) const
    {
        const int size = sizeof(object_rep);
        char* mem = static_cast<char*>(lua_newuserdata(L, size));
        return { mem, nullptr };
    }

    // this is called as metamethod __call on the class_rep.
    int class_rep::constructor_dispatcher(lua_State* L)
    {
        auto* cls = static_cast<class_rep*>(lua_touserdata(L, 1));
        int args = lua_gettop(L);
        push_new_instance(L, cls);
        if (super_deprecation_disabled
            && cls->get_class_type() == class_rep::lua_class
            && !cls->bases().empty())
        {
            lua_pushvalue(L, 1);
            lua_pushvalue(L, -2);
            lua_pushcclosure(L, super_callback, 2);
            lua_setglobal(L, "super");
        }
        lua_pushvalue(L, -1);
        lua_replace(L, 1);
        cls->get_table(L);
        lua_pushliteral(L, "__init");
        lua_gettable(L, -2);
        lua_insert(L, 1);
        lua_pop(L, 1);
        lua_insert(L, 1);
        lua_call(L, args, 0);
        if (super_deprecation_disabled)
        {
            lua_pushnil(L);
            lua_setglobal(L, "super");
        }
        return 1;
    }

    void class_rep::add_base_class(luabind::detail::class_rep* bcrep)
    {
        // If you hit this assert you are deriving from a type that is not registered
        // in lua. That is, in the class_<> you are giving a baseclass that isn't registered.
        // Please note that if you don't need to have access to the base class or the
        // conversion from the derived class to the base class, you don't need
        // to tell luabind that it derives.
        assert(bcrep && "You cannot derive from an unregistered type");
        // import all static constants
        for (auto const& scon : bcrep->m_static_constants)
        {
            int& v = m_static_constants[scon.first];
            v = scon.second;
        }
        // also, save the baseclass info to be used for typecasts
        m_bases.push_back(bcrep);
    }

    int class_rep::super_callback(lua_State* L)
    {
        int args = lua_gettop(L);
        auto* crep = static_cast<class_rep*>(lua_touserdata(L, lua_upvalueindex(1)));
        class_rep* base = crep->bases()[0];
        if (base->bases().empty())
        {
            lua_pushnil(L);
            lua_setglobal(L, "super");
        }
        else
        {
            lua_pushlightuserdata(L, base);
            lua_pushvalue(L, lua_upvalueindex(2));
            lua_pushcclosure(L, super_callback, 2);
            lua_setglobal(L, "super");
        }
        base->get_table(L);
        lua_pushliteral(L, "__init");
        lua_gettable(L, -2);
        lua_insert(L, 1);
        lua_pop(L, 1);
        lua_pushvalue(L, lua_upvalueindex(2));
        lua_insert(L, 2);
        lua_call(L, args + 1, 0);
        // TODO: instead of clearing the global variable "super"
        // store it temporarily in the registry. maybe we should
        // have some kind of warning if the super global is used?
        lua_pushnil(L);
        lua_setglobal(L, "super");
        return 0;
    }

    int class_rep::lua_settable_dispatcher(lua_State* L)
    {
        auto* crep = static_cast<class_rep*>(lua_touserdata(L, 1));
        // get first table
        crep->get_table(L);
        // copy key, value
        lua_pushvalue(L, -3);
        lua_pushvalue(L, -3);
        lua_rawset(L, -3);
        // pop table
        lua_pop(L, 1);
        // get default table
        crep->get_default_table(L);
        lua_replace(L, 1);
        lua_rawset(L, -3);
        crep->m_operator_cache = 0; // invalidate cache
        return 0;
    }

    /*
        stack:
        1: class_rep
        2: member name
    */
    int class_rep::static_class_gettable(lua_State* L)
    {
        auto* crep = static_cast<class_rep*>(lua_touserdata(L, 1));
        // look in the static function table
        crep->get_default_table(L);
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
        if (!lua_isnil(L, -1))
            return 1;

            lua_pop(L, 2);
        const char* key = lua_tostring(L, 2);
        if (!key)
        {
    #ifndef LUABIND_NO_ERROR_CHECKING
            lua_pushfstring(L, "no static value at %s-index in class '%s'",
                luaL_typename(L, 2), crep->name());
            return lua_error(L);
    #else
            lua_pushnil(L);
            return 1;
    #endif
        }
        if (std::strlen(key) != lua_rawlen(L, 2))
        {
    #ifndef LUABIND_NO_ERROR_CHECKING
            lua_pushfstring(L, "no static '%s' (followed by embedded 0) in class '%s'", key, crep->name());
            return lua_error(L);
    #else
            lua_pushnil(L);
            return 1;
    #endif
        }
        auto j = crep->m_static_constants.find(key);
        if (j != crep->m_static_constants.end())
        {
            lua_pushnumber(L, j->second);
            return 1;
        }
    #ifndef LUABIND_NO_ERROR_CHECKING

        lua_pushfstring(L, "no static '%s' in class '%s'", key, crep->name());
        return lua_error(L);

    #else
        lua_pushnil(L);
        return 1;
    #endif
    }

    int class_rep::tostring(lua_State* L)
    {
        auto* crep = static_cast<class_rep*>(lua_touserdata(L, 1));
        lua_pushfstring(L, "class %s", crep->m_name ? crep->m_name : "<unnamed>");
        if (crep->m_type != type_id())
        {
            lua_pushfstring(L, " (%s)", crep->m_type.name().c_str());
            lua_concat(L, 2);
        }
        return 1;
    }

    bool is_class_rep(lua_State* L, int index)
    {
        if (!lua_getmetatable(L, index))
            return false;
        lua_rawgetp(L, -1, &classrep_tag);
        detail::stack_pop pop(L, 2);
        return !lua_isnil(L, -1);
    }

    void finalize(lua_State* L, class_rep* crep)
    {
        if (crep->get_class_type() != class_rep::lua_class)
            return;
        crep->get_table(L);
        lua_pushliteral(L, "__finalize");
        lua_gettable(L, -2);
        lua_remove(L, -2);
        if (lua_isnil(L, -1))
            lua_pop(L, 1);
        else
        {
            lua_pushvalue(L, -2);
            lua_call(L, 1, 0);
        }
        for (auto const& bcrep : crep->bases())
            if (bcrep) finalize(L, bcrep);
    }

    void class_rep::cache_operators(lua_State* L)
    {
        m_operator_cache = 1;
        for (int i = 0; i < number_of_operators; i++)
        {
            get_table(L);
            lua_pushstring(L, get_operator_name(i));
            lua_rawget(L, -2);
            if (lua_isfunction(L, -1))
                m_operator_cache |= 1 << (i + 1);
            lua_pop(L, 2);
        }
    }

    bool class_rep::has_operator_in_lua(lua_State* L, int id)
    {
        if (!(m_operator_cache & 1))
            cache_operators(L);
        const int mask = 1 << (id + 1);
        return !!(m_operator_cache & mask);
    }
} // namespace luabind::detail
