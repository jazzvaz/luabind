// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include <luabind/lua_include.hpp>
#include <luabind/luabind.hpp>
#include <luabind/class_info.hpp>
#include <luabind/detail/class_registry.hpp>
#include <luabind/lua_argument_proxy.hpp>
#include <luabind/lua_iterator_proxy.hpp>

namespace luabind
{
    class_info get_class_info(argument const& o)
    {
        lua_State* L = o.interpreter();
        detail::class_rep* crep = nullptr;
        o.push(L);
        if (detail::is_class_rep(L, -1))
        {
            // OK, o is a class rep, now at the top of the stack
            crep = static_cast<detail::class_rep*>(lua_touserdata(L, -1));
            lua_pop(L, 1);
        }
        else // not a class rep
        {
            detail::object_rep* obj = detail::get_instance(L, -1);
            if (!obj) // not an obj rep
            {
                class_info result;
                result.name = lua_typename(L, lua_type(L, -1));
                lua_pop(L, 1);
                result.methods = newtable(L);
                result.attributes = newtable(L);
                return result;
            }
            else
            {
                lua_pop(L, 1);
                // OK, we were given an object - gotta get the crep.
                crep = obj->crep();
            }
        }
        crep->get_table(L);
        object table(from_stack(L, -1));
        lua_pop(L, 1);
        class_info result;
        result.name = detail::get_class_name(L, crep->type());
        result.methods = newtable(L);
        result.attributes = newtable(L);
        size_t index = 1;
        for (iterator i(table), e; i != e; i++)
        {
            if (type(*i) != LUA_TFUNCTION)
                continue;
            // We have to create a temporary `object` here, otherwise the proxy
            // returned by operator->() will mess up the stack. This is a known
            // problem that probably doesn't show up in real code very often.
            object member(*i);
            member.push(L);
            detail::stack_pop pop(L, 1);
            if (lua_tocfunction(L, -1) == &detail::property_tag)
                result.attributes[index++] = i.key();
            else
                result.methods[i.key()] = *i;
        }
        return result;
    }

    object get_class_names(lua_State* L)
    {
        detail::class_registry* reg = detail::class_registry::get_registry(L);
        auto const& classes = reg->get_classes();
        object result = newtable(L);
        std::size_t index = 1;
        for (auto const& [type, crep] : classes)
        {
            if (crep->name())
                result[index++] = crep->name();
        }
        return result;
    }

    void bind_class_info(lua_State* L)
    {
        detail::class_registry* reg = detail::class_registry::get_registry(L);
        if (reg->find_class(typeid(class_info)))
            return; // We've already been registered!
        module(L)
        [
            class_<class_info>("class_info_data")
                .def_readonly("name", &class_info::name)
                .def_readonly("methods", &class_info::methods)
                .def_readonly("attributes", &class_info::attributes),
            def("class_info", &get_class_info),
            def("class_names", &get_class_names)
        ];
    }
} // namespace luabind
