// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

using namespace luabind;

struct abstract
{
    virtual ~abstract() {}
    virtual luabind::string hello() = 0;
}; 

COUNTER_GUARD(abstract);

struct concrete : abstract
{
	luabind::string hello()
    {
        return "test string";
    }
};

struct abstract_wrap : abstract, wrap_base
{
	luabind::string hello()
    {
        return call_member<luabind::string>(this, "hello");
    }

    static void default_hello(abstract const&)
    {
        throw std::runtime_error("abstract function");
    }
};

luabind::string call_hello(abstract& a)
{
    return a.hello();
}

abstract& return_abstract_ref()
{
	static concrete c;
	return c;
}

abstract const& return_const_abstract_ref()
{
	static concrete c;
	return c;
}


void test_main(lua_State* L)
{
    module(L)
    [
        class_<abstract, no_bases, default_holder, abstract_wrap >("abstract")
            .def(constructor<>())
            .def("hello", &abstract::hello),

        def("call_hello", &call_hello),
		  def("return_abstract_ref", &return_abstract_ref),
		  def("return_const_abstract_ref", &return_const_abstract_ref)
    ];
    
    DOSTRING_EXPECTED(L,
        "x = abstract()\n"
        "x:hello()\n"
      , "std::runtime_error: 'Attempt to call nonexistent function: hello'");

    DOSTRING_EXPECTED(L, 
        "call_hello(x)\n"
      , "std::runtime_error: 'Attempt to call nonexistent function: hello'");
    
    DOSTRING(L,
        "class 'concrete' (abstract)\n"
        "  function concrete:__init()\n"
        "      abstract.__init(self)\n"
        "  end\n"

        "  function concrete:hello()\n"
        "      return 'hello from lua'\n"
        "  end\n");

    DOSTRING(L,
        "y = concrete()\n"
        "y:hello()\n");

    DOSTRING(L, "call_hello(y)\n");

    DOSTRING(L,
        "x = abstract()\n"
        "x.hello = function(self) return 'hello from instance' end\n"
		"assert(type(x.hello) == 'function')\n"
        "assert(x:hello() == 'hello from instance')\n"
        "assert(call_hello(x) == 'hello from instance')\n"
    );
}

