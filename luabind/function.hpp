// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

# include <luabind/make_function.hpp>
# include <luabind/scope.hpp>
# include <luabind/detail/call_function.hpp>

namespace luabind {

	namespace detail
	{

		template <class F, class PolicyInjectors>
		struct function_registration : registration
		{
			function_registration(char const* name, F f)
				: name(name)
				, f(f)
			{}

			void register_(lua_State* L, bool default_scope = false) const
			{
				object fn = make_function(L, f, default_scope, PolicyInjectors());
				add_overload(object(from_stack(L, -1)), name, fn);
			}

			char const* name;
			F f;
		};

		LUABIND_API bool is_luabind_function(lua_State* L, int index, bool allow_default = true);

	} // namespace detail

	template <class F, typename... PolicyInjectors>
	scope def(char const* name, F f, policy_list<PolicyInjectors...> const&)
	{
		return scope(luabind::unique_ptr<detail::registration>(
			luabind_new<detail::function_registration<F, policy_list<PolicyInjectors...>>>(name, f)));
	}

	template <class F>
	scope def(char const* name, F f)
	{
		return def(name, f, no_policies());
	}

} // namespace luabind
