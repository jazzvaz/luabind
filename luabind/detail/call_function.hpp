// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>

#include <luabind/error.hpp>
#include <luabind/lua_stack.hpp>
#include <luabind/detail/pcall.hpp>
#include <luabind/detail/call_shared.hpp>
#include <luabind/detail/stack_utils.hpp>

namespace luabind
{
	namespace adl {
		class object;
	}

	using adl::object;

	namespace detail {

		template< typename PolicyList, unsigned int pos >
		void push_arguments(lua_State* /*L*/) {}

		template< typename PolicyList, unsigned int Pos, typename Arg0, typename... Args >
		void push_arguments(lua_State* L, Arg0&& arg0, Args&&... args)
		{
			using converter_type = specialized_converter_policy< fetched_converter_policy<Pos, PolicyList>, Arg0, cpp_to_lua >;
			converter_type().to_lua(L, unwrapped<Arg0>::get(std::forward<Arg0>(arg0)));
			push_arguments<PolicyList, Pos + 1>(L, std::forward<Args>(args)...);
		}

		template<typename Ret, typename PolicyList, typename IndexList, unsigned int NumParams, int(*Function)(lua_State*, int, int), bool IsVoid = std::is_void<Ret>::value>
		struct call_function_struct;

		template<typename Ret, typename PolicyList, unsigned int NumParams, int(*Function)(lua_State*, int, int), unsigned int... Indices >
		struct call_function_struct< Ret, PolicyList, meta::index_list<Indices...>, NumParams, Function, true /* void */ >
		{
			template< typename... Args >
			static void call(lua_State* L, Args&&... args) {
				int top = lua_gettop(L);

				push_arguments<PolicyList, 1>(L, std::forward<Args>(args)...);

				if(Function(L, sizeof...(Args), 0)) {
					if(Function == &detail::pcall) {
						assert(lua_gettop(L) == static_cast<int>(top - NumParams + 1));
					}
					call_error(L);
				}
				// pops the return values from the function call
				stack_pop pop(L, lua_gettop(L) - top + NumParams);
			}
		};

		template<typename Ret, typename PolicyList, unsigned int NumParams, int(*Function)(lua_State*, int, int), unsigned int... Indices >
		struct call_function_struct< Ret, PolicyList, meta::index_list<Indices...>, NumParams, Function, false /* void */ >
		{
			template< typename... Args >
			static Ret call(lua_State* L, Args&&... args) {
				int top = lua_gettop(L);

				push_arguments<PolicyList, 1>(L, std::forward<Args>(args)...);

				if(Function(L, sizeof...(Args), 1)) {
					if(Function == &detail::pcall) {
						assert(lua_gettop(L) == static_cast<int>(top - NumParams + 1));
					}
					call_error(L);
				}
				// pops the return values from the function call
				stack_pop pop(L, lua_gettop(L) - top + NumParams);

				specialized_converter_policy_n<0, PolicyList, Ret, lua_to_cpp> converter;
				if(converter.match(L, decorate_type_t<Ret>(), -1) < 0 && !get_permissive_mode())
					cast_error<Ret>(L);
				return converter.to_cpp(L, decorate_type_t<Ret>(), -1);
			}
		};
	}

	template<class R, typename PolicyList = no_policies, typename... Args>
	R call_pushed_function(lua_State* L, Args&&... args)
	{
		return detail::call_function_struct<R, PolicyList, meta::index_range<1, sizeof...(Args)+1>, 1, &detail::pcall >::call(L, std::forward<Args>(args)...);
	}

	template<class R, typename PolicyList = no_policies, typename... Args>
	R call_function(lua_State* L, const char* name, Args&&... args)
	{
		assert(name && "luabind::call_function() expects a function name");
		lua_getglobal(L, name);
		return call_pushed_function<R, PolicyList>(L, std::forward<Args>(args)...);
	}

	template<class R, typename PolicyList = no_policies, typename... Args>
	R resume_pushed_function(lua_State* L, Args&&... args)
	{
		return detail::call_function_struct<R, PolicyList, meta::index_range<1, sizeof...(Args)+1>, 1, &detail::resume_impl >::call(L, std::forward<Args>(args)...);
	}

	template<class R, typename PolicyList = no_policies, typename... Args>
	R resume_function(lua_State* L, const char* name, Args&&... args)
	{
		assert(name && "luabind::resume_function() expects a function name");
		lua_getglobal(L, name);
		return resume_pushed_function<R, PolicyList>(L, std::forward<Args>(args)...);
	}

	template<class R, typename PolicyList = no_policies, typename... Args>
	R resume(lua_State* L, Args&&... args)
	{
		return detail::call_function_struct<R, PolicyList, meta::index_range<1, sizeof...(Args)+1>, 0, &detail::resume_impl >::call(L, std::forward<Args>(args)...);
	}

}
