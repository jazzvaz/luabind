// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <type_traits>
#include <luabind/detail/type_traits.hpp>

namespace luabind {

	//
	// Concept "lua_proxy"
	//

	template<class T>
	struct lua_proxy_traits
	{
		using is_specialized = std::false_type;
	};

	template<class T>
	struct is_lua_proxy_type
		: lua_proxy_traits<T>::is_specialized
	{};

	template <typename T>
	constexpr bool is_lua_proxy_type_v = is_lua_proxy_type<T>::value;

	template< class T >
	using is_lua_proxy_arg = is_lua_proxy_type<remove_const_reference_t<T>>;

	template< class T >
	constexpr bool is_lua_proxy_arg_v = is_lua_proxy_arg<T>::value;

} // namespace luabind
