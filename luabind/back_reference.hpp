// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#ifndef LUABIND_BACK_REFERENCE_040510_HPP
#define LUABIND_BACK_REFERENCE_040510_HPP

#include <luabind/config.hpp>
#include <luabind/lua_state_fwd.hpp>
#include <type_traits>
#include <luabind/wrapper_base.hpp>
#include <luabind/pointer_traits.hpp>

namespace luabind {
	struct wrap_base;

	namespace detail
	{
		template<class T>
		wrap_base const* get_back_reference_aux0(T const* p, std::true_type)
		{
			return dynamic_cast<wrap_base const*>(p);
		}

		template<class T>
		wrap_base const* get_back_reference_aux0(T const*, std::false_type)
		{
			return 0;
		}

		template<class T>
		wrap_base const* get_back_reference_aux1(T const* p)
		{
			return get_back_reference_aux0(p, std::is_polymorphic<T>());
		}

		template<class T>
		wrap_base const* get_back_reference_aux2(T const& x, std::true_type)
		{
			return get_back_reference_aux1(get_pointer(x));
		}

		template<class T>
		wrap_base const* get_back_reference_aux2(T const& x, std::false_type)
		{
			return get_back_reference_aux1(&x);
		}

		template<class T>
		wrap_base const* get_back_reference(T const& x)
		{
			return detail::get_back_reference_aux2(x, has_get_pointer<T>());
		}

	} // namespace detail

	template<class T>
	bool get_back_reference(lua_State* L, T const& x)
	{
		if(wrap_base const* w = detail::get_back_reference(x))
		{
			detail::wrap_access::ref(*w).get(L);
			return true;
		}
		return false;
	}

	template<class T>
	bool move_back_reference(lua_State* L, T const& x)
	{
		if(wrap_base* w = const_cast<wrap_base*>(detail::get_back_reference(x)))
		{
			assert(detail::wrap_access::ref(*w).m_strong_ref.is_valid());
			detail::wrap_access::ref(*w).get(L);
			detail::wrap_access::ref(*w).m_strong_ref.reset();
			return true;
		}
		return false;
	}

} // namespace luabind

#endif // LUABIND_BACK_REFERENCE_040510_HPP

