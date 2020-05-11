// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <type_traits>
#include <luabind/lua_include.hpp>
#include <luabind/detail/decorate_type.hpp>
#include <luabind/detail/make_instance.hpp>
#include <luabind/pointer_traits.hpp>
#include <luabind/from_stack.hpp>


namespace luabind {
	namespace detail {

		// Something's strange with the references here... need to know when to copy :(
		template <class T, class Clone>
		void make_pointee_instance(lua_State* L, T&& x, std::true_type, Clone)
		{
			if(get_pointer(x))
			{
				make_pointer_instance(L, std::forward<T>(x));
			}
			else
			{
				lua_pushnil(L);
			}
		}

		template <class T>
		void make_pointee_instance(lua_State* L, T&& x, std::false_type, std::true_type)
		{
			using value_type = typename std::remove_reference<T>::type;

			luabind::unique_ptr<value_type> ptr(luabind_new<value_type>(std::move(x)));
			make_pointer_instance(L, std::move(ptr));
		}

		template <class T>
		void make_pointee_instance(lua_State* L, T&& x, std::false_type, std::false_type)
		{
			// need a second make_instance that moves x into place
			make_pointer_instance(L, &x);
		}

		template <class T, class Clone>
		void make_pointee_instance(lua_State* L, T&& x, Clone)
		{
			make_pointee_instance(L, std::forward<T>(x), has_get_pointer<T>(), Clone());
		}

	}

	template <class T, class Enable>
	struct default_converter;

}
