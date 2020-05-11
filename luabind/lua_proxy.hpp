// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#ifndef LUABIND_VALUE_WRAPPER_050419_HPP
#define LUABIND_VALUE_WRAPPER_050419_HPP

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

	template< class T >
	struct is_lua_proxy_arg
		: std::conditional<is_lua_proxy_type<remove_const_reference_t<T>>::value, std::true_type, std::false_type >::type
	{};

} // namespace luabind

#endif // LUABIND_VALUE_WRAPPER_050419_HPP

