// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/detail/type_traits.hpp>
#include <luabind/detail/meta.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/conversion_policies/conversion_base.hpp>
#include <luabind/detail/conversion_policies/enum_converter.hpp>
#include <luabind/detail/conversion_policies/pointer_converter.hpp>
#include <luabind/detail/conversion_policies/reference_converter.hpp>
#include <luabind/detail/conversion_policies/value_converter.hpp>
#include <luabind/detail/conversion_policies/lua_proxy_converter.hpp>
#include <luabind/detail/conversion_policies/native_converter.hpp>
#include <luabind/detail/conversion_policies/function_converter.hpp>
#include <luabind/shared_ptr_converter.hpp>

namespace luabind {

	template <>
	struct default_converter<lua_State*>
	{
		enum { consumed_args = 0 };

		template <class U>
		lua_State* to_cpp(lua_State* L, U, int /*index*/)
		{
			return L;
		}

		template <class U>
		static int match(lua_State*, U, int /*index*/)
		{
			return 0;
		}

		template <class U>
		void converter_postcall(lua_State*, U, int) {}
	};

	namespace detail {

		// This is the one that gets hit, if default_policy doesn't hit one of the specializations defined all over the place
		template< class T >
		struct default_converter_generator
			: public meta::select_t <
			meta::case_< is_lua_proxy_arg<T>, lua_proxy_converter<T> >,
			meta::case_< std::is_enum<std::remove_reference_t<T>>, enum_converter >,
			meta::case_< is_nonconst_pointer<T>, pointer_converter >,
			meta::case_< is_const_pointer<T>, const_pointer_converter >,
			meta::case_< is_nonconst_reference<T>, ref_converter >,
			meta::case_< is_const_reference<T>, const_ref_converter >,
			meta::default_< value_converter >
			>
		{
		};

	}

	template <class T, class Enable>
	struct default_converter
		: detail::default_converter_generator<T>::type
	{};

}
