// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#pragma once

#include <luabind/detail/inheritance.hpp>
#include <luabind/detail/object_rep.hpp>

namespace luabind::detail
{
    template <class T>
    std::pair<class_id, void*> get_dynamic_class(lua_State* L, T* p)
    {
        if constexpr (std::is_polymorphic_v<T>)
        {
            lua_rawgetp(L, LUA_REGISTRYINDEX, &class_id_map_tag);
            class_id_map& class_ids = *static_cast<class_id_map*>(lua_touserdata(L, -1));
            lua_pop(L, 1);
            using nonconst_type = std::remove_const_t<T>;
            return {class_ids.get_local(typeid(*p)), dynamic_cast<void*>(const_cast<nonconst_type*>(p))};
        }
        else
            return {registered_class<T>::id, (void*)p};
    }

    template <class T>
    class_rep* get_pointee_class(class_map const& classes, T*)
    { return classes.get(registered_class<T>::id); }

    template <typename Pointee, class P>
    class_rep* get_pointee_class(lua_State* L, P const& p, class_id dynamic_id)
    {
        lua_rawgetp(L, LUA_REGISTRYINDEX, &class_map_tag);
        class_map const& classes = *static_cast<class_map*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        class_rep* cls = classes.get(dynamic_id);
        if (!cls)
            cls = get_pointee_class(classes, get_pointer(p));
        if (!cls)
            throw unresolved_name("Trying to use unregistered class", typeid(Pointee).name());
        return cls;
    }

    template <class Holder, typename... Args>
    void make_instance_internal(lua_State* L, class_rep* cls, Args&&... args)
    {
        object_rep* instance = push_new_instance(L, cls);
        void* storage = instance->allocate(sizeof(Holder));
        try
        {
            new (storage) Holder(std::forward<Args>(args)...);
        }
        catch (...)
        {
            instance->deallocate(storage);
            lua_pop(L, 1);
            throw;
        }
        instance->set_instance(static_cast<Holder*>(storage));
    }

    // Create an appropriate instance holder for the given pointer like object.
    template <class P>
    void make_pointer_instance(lua_State* L, P p)
    {
        auto [dynamic_id, dynamic_ptr] = get_dynamic_class(L, get_pointer(p));
        class_rep* cls = get_pointee_class<P>(L, p, dynamic_id);
        using holder_type = pointer_holder<std::remove_reference_t<P>>;
        make_instance_internal<holder_type>(L, cls, std::move(p), dynamic_id, dynamic_ptr);
    }

    template <typename ValueType>
    void make_value_instance_ptr(lua_State* L, ValueType&& p)
    {
        auto ptr = get_pointer(p);
        if (!ptr)
        {
            lua_pushnil(L);
            return;
        }
        auto [dynamic_id, dynamic_ptr] = get_dynamic_class(L, ptr);
        class_rep* cls = get_pointee_class<decltype(*ptr)>(L, p, dynamic_id);
        using holder_type = pointer_like_holder<std::remove_reference_t<ValueType>>;
        make_instance_internal<holder_type>(L, cls, L, std::forward<ValueType>(p), dynamic_id, dynamic_ptr);
    }

    template <typename ValueType>
    void make_value_instance_val(lua_State* L, ValueType&& v)
    {
        auto const value_type_id = detail::registered_class<ValueType>::id;
        class_rep* cls = get_pointee_class<ValueType>(L, &v, value_type_id);
        using holder_type = value_holder<std::remove_reference_t<ValueType>>;
        make_instance_internal<holder_type>(L, cls, L, std::forward<ValueType>(v));
    }

    template <typename ValueType>
    void make_value_instance(lua_State* L, ValueType&& x)
    {
        if constexpr (has_get_pointer_v<ValueType>)
            make_value_instance_ptr(L, std::forward<ValueType>(x));
        else
            make_value_instance_val(L, std::forward<ValueType>(x));
    }
} // namespace luabind::detail
