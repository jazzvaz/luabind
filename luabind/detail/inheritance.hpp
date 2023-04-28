// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <cassert>
#include <limits>
#include <map>
#include <memory>
#include <vector>
#include <luabind/typeid.hpp>

#pragma warning(push)
#pragma warning(disable : 4251)

namespace luabind::detail
{
    LUABIND_API extern char class_id_map_tag;
    LUABIND_API extern char class_map_tag;
    extern char cast_graph_tag;

    using cast_function = void* (*)(void*);
    using class_id = size_t;

    class class_rep;

    class LUABIND_API cast_graph
    {
    public:
        cast_graph();
        ~cast_graph();

        // `src` and `p` here describe the *most derived* object. This means that
        // for a polymorphic type, the pointer must be cast with
        // dynamic_cast<void*> before being passed in here, and `src` has to
        // match typeid(*p).
        std::pair<void*, int> cast(void* p, class_id src, class_id tgt, class_id d_id, void const* d_ptr) const;
        void insert(class_id src, class_id tgt, cast_function cast);

    private:
        class impl;
        luabind::unique_ptr<impl> m_impl;
    };

    // Maps a type_id to a class_id. Note that this actually partitions the
    // id-space into two, using one half for "local" ids; ids that are used only as
    // keys into the conversion cache. This is needed because we need a unique key
    // even for types that hasn't been registered explicitly.
    class LUABIND_API class_id_map
    {
    public:
        class_id_map();

        class_id get(type_id const& type) const;
        class_id get_local(type_id const& type);
        void put(class_id id, type_id const& type);

    private:
        using map_type = luabind::map<type_id, class_id>;
        map_type m_classes;
        class_id m_local_id;
        static class_id const local_id_base;
    };

    class LUABIND_API class_map
    {
    public:
        class_rep* get(class_id id) const;
        void put(class_id id, class_rep* cls);

    private:
        luabind::vector<class_rep*> m_classes;
    };

    template <class S, class T>
    struct static_cast_
    {
        static void* execute(void* p)
        {
            return static_cast<T*>(static_cast<S*>(p));
        }
    };

    template <class S, class T>
    struct dynamic_cast_
    {
        static void* execute(void* p)
        {
            return dynamic_cast<T*>(static_cast<S*>(p));
        }
    };

    // Thread safe class_id allocation.
    LUABIND_API class_id allocate_class_id(type_id cls);

    template <class T>
    struct registered_class
    {
        static inline class_id const id()
        {
            static class_id this_id = allocate_class_id(typeid(T));
            return this_id;
        } 
    };

    template <class T>
    struct registered_class<T const> :
        registered_class<T>
    {};
} // namespace luabind::detail

#pragma warning(pop) 