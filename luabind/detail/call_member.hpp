// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/lua_stack.hpp>
#include <luabind/detail/pcall.hpp>
#include <luabind/error.hpp>
#include <luabind/detail/stack_utils.hpp>
#include <luabind/detail/call_shared.hpp>
#include <luabind/object.hpp>

namespace luabind
{
	using adl::object;

	namespace detail {

		template<class R, typename PolicyList, unsigned int... Indices, typename... Args>
		R call_member_impl(lua_State* L, meta::index_list<Indices...>, Args&&... args)
		{
			// don't count the function and self-reference
			// since those will be popped by pcall
			int top = lua_gettop(L) - 2;

			// pcall will pop the function and self reference
			// and all the parameters

			(specialized_converter_policy_n<Indices, PolicyList, unwrapped_t<Args>, cpp_to_lua>().to_lua(L, unwrapped<Args>::get(std::forward<Args>(args))), ...);

			if (pcall(L, sizeof...(Args)+1, std::is_void_v<R> ? 0 : 1))
			{
				assert(lua_gettop(L) == top + 1);
				call_error(L);
			}
			// pops the return values from the function
			stack_pop pop(L, lua_gettop(L) - top);
			if constexpr (!std::is_void_v<R>)
			{
				specialized_converter_policy_n<0, PolicyList, R, lua_to_cpp> converter;
				if (converter.match(L, decorate_type_t<R>(), -1) < 0 && !get_permissive_mode())
					cast_error<R>(L);
				return converter.to_cpp(L, decorate_type_t<R>(), -1);
			}
		}

	} // detail

	template<class R, typename PolicyList = no_policies, typename... Args>
	R call_member(object const& obj, const char* name, Args&&... args)
	{
		// this will be cleaned up by the proxy object
		// once the call has been made

		// get the function
		obj.push(obj.interpreter());
		lua_pushstring(obj.interpreter(), name);
		lua_gettable(obj.interpreter(), -2);
		// duplicate the self-object
		lua_pushvalue(obj.interpreter(), -2);
		// remove the bottom self-object
		lua_remove(obj.interpreter(), -3);

		// now the function and self objects
		// are on the stack. These will both
		// be popped by pcall

		return detail::call_member_impl<R, PolicyList>(
			obj.interpreter(), meta::index_range<1, sizeof...(Args)+1>(), std::forward<Args>(args)...);
	}

	template <class R, typename... Args>
	R call_member(wrap_base const* self, char const* fn, Args&&... args)
	{
		return self->call<R>(fn, std::forward<Args>(args)...);
	}

}
