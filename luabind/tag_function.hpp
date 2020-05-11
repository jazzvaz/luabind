// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/type_traits.hpp>
#include <luabind/lua_state_fwd.hpp>

namespace luabind {

	template <class Signature, class F>
	struct tagged_function
	{
		tagged_function(F f)
			: f(f)
		{}

		F f;
	};

	namespace detail
	{

		struct invoke_context;
		struct function_object;
		
		template < typename PolicyList, typename Signature, typename F >
		int invoke_best_match(lua_State* L, function_object const& self, invoke_context& ctx, tagged_function<Signature, F> /*const*/& tagged, int args)
		{
			return invoke_best_match<PolicyList, Signature>(L, self, ctx, tagged.f, args);
		}

		template < typename PolicyList, typename Signature, typename F >
		int invoke(lua_State* L, function_object const& self, invoke_context& ctx, tagged_function<Signature, F> /*const*/& tagged)
		{
			return invoke<PolicyList, Signature>(L, self, ctx, tagged.f);
		}

	} // namespace detail

	template <class Signature, class F>
	tagged_function<deduce_signature_t<Signature>, F >
		tag_function(F f)
	{
		return f;
	}

} // namespace luabind
