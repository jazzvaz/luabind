// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#ifndef LUABIND_DIRECTORY_ITERATOR_HPP_INCLUDED
#define LUABIND_DIRECTORY_ITERATOR_HPP_INCLUDED

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/detail/implicit_cast.hpp>
#include <luabind/lua_stack.hpp>

namespace luabind {
	namespace detail
	{
		template<class Iter>
		struct dir_iterator_state
		{
			typedef dir_iterator_state<Iter> self_t;

			static int step(lua_State* L)
			{
				self_t& state = *static_cast<self_t*>(lua_touserdata(L, lua_upvalueindex(1)));

				if(state.start == state.end)
				{
					lua_pushnil(L);
					return 1;
				} else
				{
					convert_to_lua(L, *state.start);
				}

				++state.start;

				return 1;
			}

			dir_iterator_state(const Iter& s, const Iter& e)
				: start(s)
				, end(e)
			{}

			Iter start;
			Iter end;
		};

		struct dir_iterator_converter
		{
			template<class T>
			void apply(lua_State* L, const T& c)
			{
				typedef boost::filesystem::directory_iterator iter_t;
				typedef dir_iterator_state<iter_t> state_t;

				// note that this should be destructed, for now.. just hope that iterator
				// is a pod
				void* iter = lua_newuserdata(L, sizeof(state_t));
				new (iter) state_t(iter_t(c), iter_t());
				lua_pushcclosure(L, state_t::step, 1);
			}

			template<class T>
			void apply(lua_State* L, T& c)
			{
				typedef boost::filesystem::directory_iterator iter_t;
				typedef dir_iterator_state<iter_t> state_t;

				// note that this should be destructed, for now.. just hope that iterator
				// is a pod
				void* iter = lua_newuserdata(L, sizeof(state_t));
				new (iter) state_t(iter_t(c), iter_t());
				lua_pushcclosure(L, state_t::step, 1);
			}
		};

		struct dir_iterator_policy : conversion_policy<0>
		{
			static void precall(lua_State*, const index_map&) {}
			static void postcall(lua_State*, const index_map&) {}

			template<class T, class Direction>
			struct generate_converter
			{
				typedef dir_iterator_converter type;
			};
		};

	}
}

namespace luabind
{
	namespace
	{
		LUABIND_ANONYMOUS_FIX detail::policy_cons<detail::dir_iterator_policy, detail::null_type> return_directory_iterator;
	}
}

#endif // LUABIND_ITERATOR_POLICY_HPP_INCLUDED

