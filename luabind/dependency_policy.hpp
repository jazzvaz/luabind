// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>    // for policy_cons, etc
#include <luabind/detail/object_rep.hpp>  // for object_rep
#include <luabind/detail/primitives.hpp>  // for null_type

namespace luabind {
	namespace detail {

		// makes A dependent on B, meaning B will outlive A.
		// internally A stores a reference to B
		template<int A, int B>
		struct dependency_policy
		{
			template< unsigned int... StackIndices >
			static void postcall(lua_State* L, int results, meta::index_list<StackIndices...>)
			{
				object_rep* nurse = static_cast<object_rep*>(lua_touserdata(L, meta::get<meta::index_list<StackIndices...>, A>::value));

				// If the nurse isn't an object_rep, just make this a nop.
				if(nurse == 0)
					return;

				nurse->add_dependency(L, meta::get<meta::index_list<StackIndices...>, B>::value);
			}
		};

		template<int B>
		struct dependency_policy<0, B>
		{
			template< unsigned int... StackIndices >
			static void postcall(lua_State* L, int results, meta::index_list<StackIndices...>)
			{
				object_rep* nurse = static_cast<object_rep*>(lua_touserdata(L, meta::get<meta::index_list<StackIndices...>, 0>::value + results));

				// If the nurse isn't an object_rep, just make this a nop.
				if(nurse == 0)
					return;

				nurse->add_dependency(L, meta::get<meta::index_list<StackIndices...>, B>::value);
			}
		};

		template<int A>
		struct dependency_policy<A, 0>
		{
			template< unsigned int... StackIndices >
			static void postcall(lua_State* L, int results, meta::index_list<StackIndices...>)
			{
				object_rep* nurse = static_cast<object_rep*>(lua_touserdata(L, meta::get<meta::index_list<StackIndices...>, A>::value));

				// If the nurse isn't an object_rep, just make this a nop.
				if(nurse == 0)
					return;

				nurse->add_dependency(L, meta::get<meta::index_list<StackIndices...>, 0>::value + results);
			}
		};

	}
}

namespace luabind
{
	namespace policy
	{
		template<unsigned int A, unsigned int B>
		using dependency = call_policy_injector<detail::dependency_policy<A, B>>;

		template<unsigned int A>
		using return_ref = call_policy_injector<detail::dependency_policy<0, A>>;
	}
}
