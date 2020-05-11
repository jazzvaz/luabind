// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <luabind/types.hpp>
#include <stdexcept>

// TODO: Rename to pointer_traits

namespace luabind {

	template<typename T>
	T* get_pointer(T* pointer)
	{
		return pointer;
	}

	template<typename T>
	T* get_pointer(const luabind::unique_ptr<T>& pointer)
	{
		return pointer.get();
	}

	template<typename T>
	T* get_pointer(const std::shared_ptr<T>& pointer)
	{
		return pointer.get();
	}


	namespace detail {

		template<typename T>
		struct pointer_traits {
			enum { is_pointer = false };
		};

		template<typename T>
		struct pointer_traits<T*>
		{
			enum { is_pointer = true };
			using value_type = T;
		};

		template<typename T>
		struct pointer_traits<luabind::unique_ptr<T>>
		{
			enum { is_pointer = true };
			using value_type = T;
		};

		template<typename T>
		struct pointer_traits<std::shared_ptr<T>>
		{
			enum { is_pointer = true };
			using value_type = T;
		};

		template<typename T>
		using is_pointer_to_const = std::is_const< typename pointer_traits<T>::value_type >;

		template<typename T>
		void release_ownership(luabind::unique_ptr<T>& p)
		{
			p.release();
		}

		template <class P>
		void release_ownership(P const&)
		{
			throw std::runtime_error(
				"luabind: smart pointer does not allow ownership transfer");
		}

		namespace has_get_pointer_
		{

			struct any
			{
				template<class T> any(T const&);
			};

			struct no_overload_tag
			{};

			typedef char(&yes)[1];
			typedef char(&no)[2];

			no_overload_tag operator, (no_overload_tag, int);

			// required for unqualified name lookup inside impl for shared_ptr<T>
			// (otherwise we would need to put it either in global or std namespace)
			using luabind::get_pointer;

			detail::has_get_pointer_::no_overload_tag
				get_pointer(detail::has_get_pointer_::any);

			///@TODO: Rework
			template<class T>
			yes check(T const&);
			no check(no_overload_tag);

			template<class T>
			struct impl
			{
				static typename std::add_lvalue_reference<T>::type x;
				static const bool value = (sizeof(has_get_pointer_::check((get_pointer(x), 0))) == 1);
				typedef std::integral_constant<bool, value> type;
			};

		} // namespace has_get_pointer_

		template<class T>
		struct has_get_pointer
			: has_get_pointer_::impl<T>::type
		{};

	} // namespace detail

} // namespace luabind
