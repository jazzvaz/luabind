// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/scope.hpp>

namespace {

    struct X {};
    struct Y {};
    
} // namespace unnamed

luabind::scope test_separate_registration()
{
    using namespace luabind;

    return class_<X>("X")
               .def(constructor<>()),
           class_<Y>("Y")
               .def(constructor<>())
           ;
}

TEST_CASE("separation")
{
    using namespace luabind;
    
    module(L)
    [
        namespace_("Z") [
            test_separate_registration()
        ]
    ];

    DOSTRING(L,"x = Z.X()");
    DOSTRING(L,"y = Z.Y()");
}

