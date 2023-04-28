// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <luabind/get_main_thread.hpp>
#include <luabind/lua_argument_proxy.hpp>
#include <luabind/wrapper_base.hpp>
#include <luabind/detail/inheritance.hpp>

namespace luabind::detail
{
    inline void inject_backref(lua_State*, void*, void*) {}

    template <class T>
    void inject_backref(lua_State* L, T* p, wrap_base*)
    {
        weak_ref(get_main_thread(L), L, 1).swap(wrap_access::ref(*p));
    }

    template <class T, class Pointer, class Signature, class Args, class ArgIndices>
    struct construct_aux_helper;

    template <class T, class Pointer, class Signature, typename... Args, uint32_t... ArgIndices>
    struct construct_aux_helper<T, Pointer, Signature, meta::type_list<Args...>, meta::index_list<ArgIndices...>>
    {
        using holder_type = pointer_holder<Pointer, T>;

        void operator()(argument const& self_, Args... args) const
        {
            object_rep* self = touserdata<object_rep>(self_);
            luabind::unique_ptr<T> instance(luabind_new<T>(args...));
            inject_backref(self_.interpreter(), instance.get(), instance.get());
            void* naked_ptr = instance.get();
            Pointer ptr(instance.release());
            void* storage = self->allocate(sizeof(holder_type));
            self->set_instance(new (storage) holder_type(std::move(ptr), registered_class<T>::id(), naked_ptr));
        }
    };

    template <class T, class Pointer, class Signature>
    struct construct :
        construct_aux_helper<T, Pointer, Signature,
        meta::sub_range<Signature, 2, meta::size_v<Signature>>,
        meta::index_range<0, meta::size_v<Signature> - 2>>
    {};
}    // namespace luabind::detail
