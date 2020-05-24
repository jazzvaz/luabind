// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <luabind/lua_proxy.hpp>
#include <type_traits>

namespace luabind::detail
{
	template <class U>
	struct lua_proxy_converter
	{
		using type = lua_proxy_converter<U>;
		using is_native = std::true_type;

		static constexpr int consumed_args = 1;

		template <class T>
		T to_cpp(lua_State* L, by_const_reference<T>, int index)
		{ return T(from_stack(L, index)); }

		template <class T>
		T to_cpp(lua_State* L, by_value<T>, int index)
		{ return to_cpp(L, by_const_reference<T>(), index); }

		template <class T>
		int match(lua_State* L, by_const_reference<T>, int index)
		{
			if (lua_proxy_traits<T>::check(L, index))
				return max_hierarchy_depth;
			else
				return no_match;
		}

		template <class T>
		int match(lua_State* L, by_value<T>, int index)
		{ return match(L, by_const_reference<T>(), index); }

		void converter_postcall(...) {}

		template <class T>
		void to_lua(lua_State* interpreter, T const& value_wrapper)
		{ lua_proxy_traits<T>::unwrap(interpreter, value_wrapper); }
	};
} // namespace luabind::detail
