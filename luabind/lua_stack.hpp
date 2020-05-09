// Copyright (c) 2003 Daniel Wallin and Arvid Norberg

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef LUABIND_CONVERT_TO_LUA_HPP_INCLUDED
#define LUABIND_CONVERT_TO_LUA_HPP_INCLUDED

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/type_traits.hpp>
#include <luabind/detail/stack_utils.hpp>
#include <luabind/error.hpp> // call_shared
#include <luabind/detail/call_shared.hpp> // cast_error

namespace luabind {

	namespace detail {

		template< typename T >
		struct unwrapped {
			static const bool is_wrapped_ref = false;
			using type = T;

			static const T& get(const T& t) {
				return t;
			}
		};

		template< typename T >
		struct unwrapped< std::reference_wrapper< T > >
		{
			static const bool is_wrapped_ref = true;
			using type = T&;

			static T& get(const std::reference_wrapper<T>& refwrap)
			{
				return refwrap.get();
			}
		};

		template<typename T>
		using unwrapped_t = typename unwrapped< T >::type;
	}

	namespace lua_stack
	{
		template<typename Policies = no_policies, int PolicyIndex = 1, typename T>
		void push(lua_State* L, T&& v)
		{
			using namespace detail;
			using value_type = unwrapped_t<remove_const_reference_t<T>>;
			specialized_converter_policy_n<PolicyIndex, Policies, value_type, cpp_to_lua> cv;
			cv.to_lua(L, unwrapped<T>::get(v));
		}

		template<typename T, typename Policies = no_policies, int PolicyIndex = 1>
		T pop(lua_State* L)
		{
			using namespace detail;
			using value_type = unwrapped_t<T>;
			stack_pop pop(L, 1);
			specialized_converter_policy_n<PolicyIndex, Policies, value_type, lua_to_cpp> cv;
			if (cv.match(L, decorate_type_t<T>(), -1) < 0)
			{
#ifndef LUABIND_PERMISSIVE_MODE
				cast_error<T>(L);
#endif
			}
			return cv.to_cpp(L, decorate_type_t<T>(), -1);
		}

		template<>
		inline void pop<void, no_policies, 1>(lua_State* L)
		{ detail::stack_pop pop(L, 1); }

		inline void pop(lua_State* L, int n = 1)
		{ detail::stack_pop pop(L, n); }

	} // namespace lua_stack
}

#endif
