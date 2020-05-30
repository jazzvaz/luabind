// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

namespace luabind::detail
{
    template <class Class, class T, class Result = T>
    struct access_member_ptr
    {
        access_member_ptr(T Class::* mem_ptr) :
            mem_ptr(mem_ptr)
        {}

        Result operator()(Class const& x) const
        { return const_cast<Class&>(x).*mem_ptr; }

        void operator()(Class& x, T const& value) const
        { x.*mem_ptr = value; }

        T Class::* mem_ptr;
    };
} // namespace luabind::detail
