// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/decorate_type.hpp>  // for decorated_type
#include <luabind/detail/primitives.hpp>  // for null_type (ptr only), etc

namespace luabind {
	namespace detail {

		template<class Policies>
		struct container_converter_lua_to_cpp
		{
			enum { consumed_args = 1 };

			template<class T>
			T to_cpp(lua_State* L, by_const_reference<T>, int index)
			{
				using value_type = typename T::value_type;
				specialized_converter_policy_n<1, Policies, value_type, lua_to_cpp> converter;

				T container;

				lua_pushnil(L);
				while(lua_next(L, index - 1))
				{
					container.push_back(converter.apply(L, decorated_type<value_type>(), -1));
					lua_pop(L, 1); // pop value
				}

				return container;
			}

			template<class T>
			T to_cpp(lua_State* L, by_value<T>, int index)
			{
				return to_cpp(L, by_const_reference<T>(), index);
			}

			template<class T>
			static int match(lua_State* L, by_const_reference<T>, int index)
			{
				if(lua_istable(L, index)) return 0; else return no_match;
			}

			template<class T>
			static int match(lua_State* L, by_value<T>, int index)
			{
				return match(L, by_const_reference<T>(), index);
			}

			template<class T>
			void converter_postcall(lua_State*, T, int) {}
		};

		template<class Policies>
		struct container_converter_cpp_to_lua
		{
			template<class T>
			void to_lua(lua_State* L, const T& container)
			{
				using value_type = typename T::value_type;
				specialized_converter_policy_n<1, Policies, value_type, lua_to_cpp> converter;

				lua_newtable(L);

				int index = 1;

				for(const auto& element : container)
				{
					converter.apply(L, element);
					lua_rawseti(L, -2, index);
					++index;
				}
			}
		};

		template<class Policies = no_policies>
		struct container_policy
		{
			struct only_accepts_nonconst_pointers {};

			template<class T, class Direction>
			struct specialize;

			template<class T>
			struct specialize<T, lua_to_cpp> {
				using type = container_converter_lua_to_cpp<Policies>;
			};

			template<class T>
			struct specialize<T, cpp_to_lua> {
				using type = container_converter_cpp_to_lua<Policies>;
			};
		};

	}
}

namespace luabind
{
	namespace policy
	{
		template<unsigned int N, typename ElementPolicies = no_policies >
		using container = converter_policy_injector<N, detail::container_policy<ElementPolicies>>;
	}
}
