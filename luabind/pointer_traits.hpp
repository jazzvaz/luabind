// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <luabind/types.hpp>
#include <stdexcept>

namespace luabind
{
	template <typename T>
	T* get_pointer(T* pointer)
	{ return pointer; }

	template <typename T>
	T* get_pointer(const luabind::unique_ptr<T>& pointer)
	{ return pointer.get(); }

	template <typename T>
	T* get_pointer(const std::shared_ptr<T>& pointer)
	{ return pointer.get(); }
} // namespace luabind

namespace luabind::detail
{
	template <typename T>
	struct pointer_traits
	{
		static constexpr bool is_pointer = false;
	};

	template <typename T>
	struct pointer_traits<T*>
	{
		static constexpr bool is_pointer = true;
		using value_type = T;
	};

	template <typename T>
	struct pointer_traits<luabind::unique_ptr<T>>
	{
		static constexpr bool is_pointer = true;
		using value_type = T;
	};

	template <typename T>
	struct pointer_traits<std::shared_ptr<T>>
	{
		static constexpr bool is_pointer = true;
		using value_type = T;
	};

	template <typename T>
	using is_pointer_to_const = std::is_const<typename pointer_traits<T>::value_type>;

	template <typename T>
	void release_ownership(luabind::unique_ptr<T>& p) { p.release(); }

	template <class P>
	void release_ownership(P const&)
	{
		throw std::runtime_error("luabind: smart pointer does not allow ownership transfer");
	}

	template <typename T, typename = void>
	struct has_get_pointer : std::false_type
	{};

	template <typename T>
	struct has_get_pointer<T, std::void_t<decltype(get_pointer(std::declval<T>()))>> : std::true_type
	{};

	template <typename T>
	constexpr bool has_get_pointer_v = has_get_pointer<T>::value;
} // namespace luabind::detail
