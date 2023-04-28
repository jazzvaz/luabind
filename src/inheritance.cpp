// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include <limits>
#include <luabind/typeid.hpp>
#include <luabind/detail/inheritance.hpp>

namespace luabind::detail
{
    char class_id_map_tag;
    char class_map_tag;
    char cast_graph_tag;

    class_id const class_id_map::local_id_base = std::numeric_limits<class_id>::max() / 2;

    struct edge
    {
        edge(class_id target, cast_function cast) :
            target(target),
            cast(cast)
        {}

        class_id target;
        cast_function cast;
    };

    static bool operator<(edge const& x, edge const& y)
    {
        return x.target < y.target;
    }

    struct vertex
    {
        vertex(class_id id) :
            id(id)
        {}

        class_id id;
        luabind::vector<edge> edges;
    };

    using cache_entry = std::pair<ptrdiff_t, int>;

    class cache
    {
    public:
        static constexpr ptrdiff_t unknown = std::numeric_limits<ptrdiff_t>::max();
        static constexpr ptrdiff_t invalid = unknown - 1;

        cache_entry get(class_id src, class_id tgt, class_id d_id, ptrdiff_t obj_offset) const
        {
            auto i = m_cache.find(key_type(src, tgt, d_id, obj_offset));
            return i != m_cache.end() ? i->second : cache_entry(unknown, -1);
        }

        void put(class_id src, class_id tgt, class_id d_id, ptrdiff_t obj_offset, ptrdiff_t offset, int distance)
        {
            m_cache.emplace(key_type(src, tgt, d_id, obj_offset), cache_entry(offset, distance));
        }

        void invalidate()
        {
            m_cache.clear();
        }

    private:
        using key_type = std::tuple<class_id, class_id, class_id, ptrdiff_t>;
        luabind::map<key_type, cache_entry> m_cache;
    };

    struct queue_entry
    {
        queue_entry(void* p, class_id vertex_id, int distance) :
            p(p),
            vertex_id(vertex_id),
            distance(distance)
        {}

        void* p;
        class_id vertex_id;
        int distance;
    };

    class cast_graph::impl
    {
    public:
        std::pair<void*, int> cast(void* p, class_id src, class_id tgt, class_id d_id, void const* d_ptr) const
        {
            if (src == tgt)
                return { p, 0 };
            if (src >= m_vertices.size() || tgt >= m_vertices.size())
                return { nullptr, -1 };
            ptrdiff_t const obj_offset = (char const*)d_ptr - (char const*)p;
            cache_entry cached = m_cache.get(src, tgt, d_id, obj_offset);
            if (cached.first != cache::unknown)
            {
                if (cached.first == cache::invalid)
                    return { nullptr, -1 };
                return { (char*)p + cached.first, cached.second };
            }
            luabind::queue<queue_entry> q;
            q.push(queue_entry(p, src, 0));
            // Original source used boost::dynamic_bitset but didn't make use
            // of its advanced capability of set operations, that's why I think
            // it's safe to use a vector<bool> here.
            luabind::vector<bool> visited(m_vertices.size(), false);
            while (!q.empty())
            {
                queue_entry const qe = q.front();
                q.pop();
                visited[qe.vertex_id] = true;
                vertex const& v = m_vertices[qe.vertex_id];
                if (v.id == tgt)
                {
                    m_cache.put(src, tgt, d_id, obj_offset, (char*)qe.p - (char*)p, qe.distance);
                    return { qe.p, qe.distance };
                }
                for (auto const& e : v.edges)
                {
                    if (visited[e.target])
                        continue;
                    if (void* casted = e.cast(qe.p))
                        q.push(queue_entry(casted, e.target, qe.distance + 1));
                }
            }
            m_cache.put(src, tgt, d_id, obj_offset, cache::invalid, -1);
            return { nullptr, -1 };
        }

        void insert(class_id src, class_id tgt, cast_function cast)
        {
            class_id const max_id = std::max(src, tgt);
            if (max_id >= m_vertices.size())
            {
                m_vertices.reserve(max_id + 1);
                for (class_id i = m_vertices.size(); i < max_id + 1; ++i)
                    m_vertices.emplace_back(i);
            }
            luabind::vector<edge>& edges = m_vertices[src].edges;
            auto i = std::lower_bound(edges.begin(), edges.end(), edge(tgt, 0));
            if (i == edges.end() || i->target != tgt)
            {
                edges.emplace(i, tgt, cast);
                m_cache.invalidate();
            }
        }

    private:
        luabind::vector<vertex> m_vertices;
        mutable cache m_cache;
    };

    std::pair<void*, int> cast_graph::cast(void* p, class_id src, class_id tgt,
        class_id d_id, void const* d_ptr) const
    {
        return m_impl->cast(p, src, tgt, d_id, d_ptr);
    }

    void cast_graph::insert(class_id src, class_id tgt, cast_function cast)
    {
        m_impl->insert(src, tgt, cast);
    }

    cast_graph::cast_graph() :
        m_impl(luabind_new<impl>())
    {}

    cast_graph::~cast_graph() {}

    class_id allocate_class_id(type_id const& cls)
    {
        // use plain map here because this function is called by static initializers,
        // so luabind::allocator is not set yet
        using map_type = std::map<type_id, class_id>;
        static map_type registered;
        static class_id id = 0;
        auto [it, inserted] = registered.emplace(cls, id);
        if (inserted)
            id++;
        return it->second;
    }
} // namespace luabind::detail
