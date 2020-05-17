// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <type_traits>
#include <string>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/conversion_policies/conversion_base.hpp>
#include <luabind/detail/type_traits.hpp>
#include <luabind/detail/call_traits.hpp>
#include <luabind/lua_include.hpp>

namespace luabind {

	template <class T, class Derived = default_converter<T> >
	struct native_converter_base
	{
		using is_native = std::true_type;
		using value_type = typename detail::call_traits<T>::value_type;
		using param_type = typename detail::call_traits<T>::param_type;

		enum { consumed_args = 1 };

		template <class U>
		void converter_postcall(lua_State*, U const&, int)
		{}

		int match(lua_State* L, by_value<T>, int index)
		{
			return Derived::compute_score(L, index);
		}

		int match(lua_State* L, by_value<T const>, int index)
		{
			return Derived::compute_score(L, index);
		}


		int match(lua_State* L, by_const_reference<T>, int index)
		{
			return Derived::compute_score(L, index);
		}

		value_type to_cpp(lua_State* L, by_value<T>, int index)
		{
			return derived().to_cpp_deferred(L, index);
		}

		value_type to_cpp(lua_State* L, by_const_reference<T>, int index)
		{
			return derived().to_cpp_deferred(L, index);
		}

		void to_lua(lua_State* L, param_type value)
		{
			derived().to_lua_deferred(L, value);
		}

		Derived& derived()
		{
			return static_cast<Derived&>(*this);
		}
	};

	template <typename QualifiedT>
	struct integer_converter
		: native_converter_base<remove_const_reference_t<QualifiedT>>
	{
		using T = remove_const_reference_t<QualifiedT>;
		using value_type = typename native_converter_base<T>::value_type;
		using param_type = typename native_converter_base<T>::param_type;

		static int compute_score(lua_State* L, int index)
		{
			return lua_type(L, index) == LUA_TNUMBER ? 0 : no_match;
		}

		static value_type to_cpp_deferred(lua_State* L, int index)
		{
			if((std::is_unsigned_v<value_type> && sizeof(value_type) >= sizeof(lua_Integer)) || (sizeof(value_type) > sizeof(lua_Integer))) {
				return static_cast<T>(lua_tonumber(L, index));
			} else {
				return static_cast<T>(lua_tointeger(L, index));
			}
		}

		void to_lua_deferred(lua_State* L, param_type value)
		{
			if((std::is_unsigned_v<value_type> && sizeof(value_type) >= sizeof(lua_Integer)) || (sizeof(value_type) > sizeof(lua_Integer)))
			{
				lua_pushnumber(L, (lua_Number)value);
			} else {
				lua_pushinteger(L, static_cast<lua_Integer>(value));
			}
		}
	};

	template <typename QualifiedT>
	struct number_converter
		: native_converter_base<remove_const_reference_t<QualifiedT>>
	{
		using T = remove_const_reference_t<QualifiedT>;
		using value_type = typename native_converter_base<T>::value_type;
		using param_type = typename native_converter_base<T>::param_type;

		static int compute_score(lua_State* L, int index)
		{
			return lua_type(L, index) == LUA_TNUMBER ? 0 : no_match;
		}

		static value_type to_cpp_deferred(lua_State* L, int index)
		{
			return static_cast<T>(lua_tonumber(L, index));
		}

		static void to_lua_deferred(lua_State* L, param_type value)
		{
			lua_pushnumber(L, static_cast<lua_Number>(value));
		}
	};

	template <>
	struct default_converter<bool>
		: native_converter_base<bool>
	{
		static int compute_score(lua_State* L, int index)
		{
			return lua_type(L, index) == LUA_TBOOLEAN ? 0 : no_match;
		}

		static bool to_cpp_deferred(lua_State* L, int index)
		{
			return lua_toboolean(L, index) == 1;
		}

		static void to_lua_deferred(lua_State* L, bool value)
		{
			lua_pushboolean(L, value);
		}
	};


	template <>
	struct default_converter<bool const>
		: default_converter<bool>
	{};

	template <>
	struct default_converter<bool const&>
		: default_converter<bool>
	{};

	template <>
	struct default_converter<luabind::string>
		: native_converter_base<luabind::string>
	{
		static int compute_score(lua_State* L, int index)
		{
			int type = lua_type(L, index);
			switch (type)
			{
			case LUA_TSTRING:
				return 0;
			case LUA_TNUMBER:
				if (get_permissive_mode())
					return 1;
			default:
				return no_match;
			}
		}

		static luabind::string to_cpp_deferred(lua_State* L, int index)
		{
			return luabind::string(lua_tostring(L, index), lua_rawlen(L, index));
		}

		static void to_lua_deferred(lua_State* L, luabind::string const& value)
		{
			lua_pushlstring(L, value.data(), value.size());
		}
	};

	template <>
	struct default_converter<luabind::string&>
		: default_converter<luabind::string>
	{};

	template <>
	struct default_converter<luabind::string const>
		: default_converter<luabind::string>
	{};

	template <>
	struct default_converter<luabind::string const&>
		: default_converter<luabind::string>
	{};

	template <>
	struct default_converter<char const*>
	{
		using is_native = std::true_type;

		enum { consumed_args = 1 };

		template <class U>
		static int match(lua_State* L, U, int index)
		{
			int type = lua_type(L, index);
			switch (type)
			{
			case LUA_TSTRING:
				return 0;
			case LUA_TNIL:
				if (is_nil_conversion_allowed())
					return 0;
			case LUA_TNUMBER:
				if (get_permissive_mode())
					return 1;
			}
			return no_match;
		}

		template <class U>
		static char const* to_cpp(lua_State* L, U, int index)
		{
			return lua_tostring(L, index);
		}

		static void to_lua(lua_State* L, char const* str)
		{
			lua_pushstring(L, str);
		}

		template <class U>
		void converter_postcall(lua_State*, U, int)
		{}
	};

	template <>
	struct default_converter<const char* const>
		: default_converter<char const*>
	{};

	template <>
	struct default_converter<const char* const&>
		: default_converter<char const*>
	{};

	template <>
	struct default_converter<const char*&>
		: default_converter<char const*>
	{};

	template <>
	struct default_converter<char*>
		: default_converter<char const*>
	{};

	template <std::size_t N>
	struct default_converter<char const[N]>
		: default_converter<char const*>
	{};

	template <std::size_t N>
	struct default_converter<char[N]>
		: default_converter<char const*>
	{};

	template <std::size_t N>
	struct default_converter <char(&)[N]>
		: default_converter<char const*>
	{};

	template <std::size_t N>
	struct default_converter <const char(&)[N]>
		: default_converter<char const*>
	{};

	template <typename T>
	struct default_converter < T, std::enable_if_t< std::is_integral_v<remove_const_reference_t<T>> > >
		: integer_converter<T>
	{
	};

	template <typename T>
	struct default_converter < T, std::enable_if_t< std::is_floating_point_v<remove_const_reference_t<T>> > >
		: number_converter<T>
	{
	};

}
