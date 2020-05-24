// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <type_traits>
#include <luabind/lua_include.hpp>
#include <luabind/detail/decorate_type.hpp>
#include <luabind/detail/make_instance.hpp>
#include <luabind/pointer_traits.hpp>
#include <luabind/from_stack.hpp>

namespace luabind::detail
{
	template <class T, class Clone>
	void make_pointee_instance(lua_State* L, T&& x, Clone)
	{
		if constexpr (has_get_pointer_v<T>)
		{
			// Something's strange with the references here... need to know when to copy :(
			if (get_pointer(x))
				make_pointer_instance(L, std::forward<T>(x));
			else
				lua_pushnil(L);
		}
		else
		{
			if constexpr (Clone::value)
			{
				using value_type = std::remove_reference_t<T>;
				luabind::unique_ptr<value_type> ptr(luabind_new<value_type>(std::move(x)));
				make_pointer_instance(L, std::move(ptr));
			}
			else
			{
				// need a second make_instance that moves x into place
				make_pointer_instance(L, &x);
			}
		}
	}
} // namespace luabind::detail

namespace luabind
{
	template <class T, class Enable>
	struct default_converter;
} // namespace luabind
