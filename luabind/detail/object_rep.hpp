// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/class_rep.hpp>
#include <luabind/detail/instance_holder.hpp>
#include <type_traits>    // std::aligned_storage
#include <cstdlib>

#pragma warning(push)
#pragma warning(disable : 4251)

namespace luabind::detail
{
    void finalize(lua_State* L, class_rep* crep);

    // this class is allocated inside lua for each pointer.
    // it contains the actual c++ object-pointer.
    // it also tells if it is const or not.
    class LUABIND_API object_rep
    {
    public:
        object_rep(instance_holder* instance, class_rep* crep);
        ~object_rep();

        const class_rep* crep() const { return m_classrep; }
        class_rep* crep() { return m_classrep; }

        void set_instance(instance_holder* instance) { m_instance = instance; }

        void add_dependency(lua_State* L, int index);

        std::pair<void*, int> get_instance(class_id target) const
        {
            if (!m_instance)
                return { nullptr, -1 };
            return m_instance->get(m_classrep->casts(), target);
        }

        bool is_const() const
        {
            return m_instance && m_instance->pointee_const();
        }

        void release()
        {
            if (m_instance)
                m_instance->release();
        }

        void* allocate(std::size_t size)
        {
            if (size <= instance_buffer_size)
                return &m_instance_buffer;
#ifdef LUABIND_CUSTOM_ALLOCATOR
            return call_allocator(nullptr, size);
#else
            return std::malloc(size);
#endif
        }

        void deallocate(void* storage)
        {
            if (storage == &m_instance_buffer)
                return;
#ifdef LUABIND_CUSTOM_ALLOCATOR
            call_allocator(storage, 0);
#else
            std::free(storage);
#endif
        }

    private:
        object_rep(object_rep const&) = delete;
        void operator=(object_rep const&) = delete;

        instance_holder* m_instance;
        static const size_t instance_buffer_size = 32;
        std::aligned_storage_t<instance_buffer_size> m_instance_buffer;
        class_rep* m_classrep; // the class information about this object's type
        handle m_dependency_ref; // reference to lua table holding dependency references
    };

    LUABIND_API object_rep* get_instance(lua_State* L, int index);
    LUABIND_API void push_instance_metatable(lua_State* L);
    LUABIND_API object_rep* push_new_instance(lua_State* L, class_rep* cls);
}    // namespace luabind::detail

#pragma warning(pop) 
