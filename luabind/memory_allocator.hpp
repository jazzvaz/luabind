// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2019 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/memory.hpp>

namespace luabind
{
#ifndef LUABIND_CUSTOM_ALLOCATOR
    template <class T>
    using memory_allocator = std::allocator<T>;
#else
    template <class T>
    class memory_allocator
    {
    private:
        using self_type = memory_allocator<T>;

    public:
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using value_type = T;

        memory_allocator() {}
        memory_allocator(memory_allocator<T> const&) {}
        template <class T2>
        memory_allocator(memory_allocator<T2> const&) {}

        template <class T2>
        memory_allocator<T>& operator=(memory_allocator<T2> const&)
        {
            return *this;
        }

        pointer address(reference value) const
        {
            return &value;
        }

        const_pointer address(const_reference value) const
        {
            return &value;
        }

        pointer allocate(size_type n, void const* p = nullptr) const
        {
            pointer result = (pointer)detail::call_allocator(p, n * sizeof(T));
            if (!n)
                result = (pointer)detail::call_allocator(p, sizeof(T));
            return result;
        }

        void deallocate(pointer p, size_type) const
        {
            detail::call_allocator(p, 0);
        }

        void deallocate(void* p, size_type) const
        {
            detail::call_allocator(p, 0);
        }

        void construct(pointer p, T const& value)
        {
            new(p) T(value);
        }

        void destroy(pointer p)
        {
            p->~T();
        }

        size_type max_size() const
        {
            size_type count = (size_type)(-1) / sizeof(T);
            if (count)
                return count;
            return 1;
        }
    };

    template <class T1, class T2>
    bool operator==(memory_allocator<T1> const&, memory_allocator<T2> const&)
    {
        return true;
    }

    template <class T1, class T2>
    bool operator!=(memory_allocator<T1> const&, memory_allocator<T2> const&)
    {
        return false;
    }
#endif
} // namespace luabind
