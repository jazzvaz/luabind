// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2007 The Luabind Authors

#pragma once

#include <luabind/config.hpp>           // for LUABIND_ANONYMOUS_FIX
#include <luabind/lua_stack.hpp>  // for lua_stack::push
#include <luabind/detail/policy.hpp>    // for index_map, etc
#include <new>                          // for operator new

namespace luabind {
	namespace detail {

		template <class Iterator>
		struct iterator
		{
			static int next(lua_State* L)
			{
				iterator* self = static_cast<iterator*>(
					lua_touserdata(L, lua_upvalueindex(1)));

				if(self->first != self->last)
				{
					lua_stack::push(L, *self->first);
					++self->first;
				} else
				{
					lua_pushnil(L);
				}

				return 1;
			}

			static int destroy(lua_State* L)
			{
				iterator* self = static_cast<iterator*>(lua_touserdata(L, 1));
				self->~iterator();
				return 0;
			}

			iterator(Iterator first, Iterator last)
				: first(first)
				, last(last)
			{}

			Iterator first;
			Iterator last;
		};

		template <class Iterator>
		int make_range(lua_State* L, Iterator first, Iterator last)
		{
			void* storage = lua_newuserdata(L, sizeof(iterator<Iterator>));
			lua_createtable(L, 0, 1);
			lua_pushcclosure(L, iterator<Iterator>::destroy, 0);
			lua_setfield(L, -2, "__gc");
			lua_setmetatable(L, -2);
			lua_pushcclosure(L, iterator<Iterator>::next, 1);
			new (storage) iterator<Iterator>(first, last);
			return 1;
		}

		template <class Container>
		int make_range(lua_State* L, Container& container)
		{
			return make_range(L, container.begin(), container.end());
		}

		struct iterator_converter
		{
			using type = iterator_converter;

			template <class Container>
			void to_lua(lua_State* L, Container& container)
			{
				make_range(L, container);
			}

			template <class Container>
			void tu_lua(lua_State* L, Container const& container)
			{
				make_range(L, container);
			}
		};

		struct iterator_policy
		{
			template <class T, class Direction>
			struct specialize
			{
				static_assert(std::is_same_v<Direction, cpp_to_lua>, "Iterator policy can only convert from cpp to lua.");
				using type = iterator_converter;
			};
		};

		template <class Iterator>
		struct iterator_pair
		{
			static int next(lua_State* L)
			{
				iterator_pair* self = static_cast<iterator_pair*>(
					lua_touserdata(L, lua_upvalueindex(1)));

				if (self->first != self->last)
				{
					lua_stack::push(L, (*self->first).first);
					lua_stack::push(L, (*self->first).second);
					++self->first;
					return 2;
				}
				else
				{
					lua_pushnil(L);
				}

				return 1;
			}

			static int destroy(lua_State* L)
			{
				iterator_pair* self = static_cast<iterator_pair*>(lua_touserdata(L, 1));
				self->~iterator_pair();
				return 0;
			}

			iterator_pair(Iterator first, Iterator last)
				: first(first)
				, last(last)
			{}

			Iterator first;
			Iterator last;
		};

		template <class Iterator>
		int make_range_pair(lua_State* L, Iterator first, Iterator last)
		{
			void* storage = lua_newuserdata(L, sizeof(iterator_pair<Iterator>));
			lua_createtable(L, 0, 1);
			lua_pushcclosure(L, iterator_pair<Iterator>::destroy, 0);
			lua_setfield(L, -2, "__gc");
			lua_setmetatable(L, -2);
			lua_pushcclosure(L, iterator_pair<Iterator>::next, 1);
			new (storage) iterator_pair<Iterator>(first, last);
			return 1;
		}

		template <class Container>
		int make_range_pair(lua_State* L, Container& container)
		{
			return make_range_pair(L, container.begin(), container.end());
		}

		struct iterator_pair_converter
		{
			using type = iterator_pair_converter;

			template <class Container>
			void to_lua(lua_State* L, Container& container)
			{
				make_range_pair(L, container);
			}

			template <class Container>
			void tu_lua(lua_State* L, Container const& container)
			{
				make_range_pair(L, container);
			}
		};

		struct iterator_pair_policy
		{
			template <class T, class Direction>
			struct specialize
			{
				static_assert(std::is_same_v<Direction, cpp_to_lua>, "Iterator policy can only convert from cpp to lua.");
				using type = iterator_pair_converter;
			};
		};

	} // namespace detail
} // namespace luabind

namespace luabind
{
	namespace policy
	{
		using return_stl_iterator = converter_policy_injector<0, detail::iterator_policy>;
		using return_stl_pair_iterator = converter_policy_injector<0, detail::iterator_pair_policy>;
	}
} // namespace luabind
