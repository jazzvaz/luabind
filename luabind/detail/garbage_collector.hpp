// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#ifndef LUABIND_GARBAGE_COLLECTOR_HPP_INCLUDED
#define LUABIND_GARBAGE_COLLECTOR_HPP_INCLUDED

#include <luabind/config.hpp>

namespace luabind {
	namespace detail {

		// function that is used as __gc metafunction on several objects
		template<class T>
		inline int garbage_collector(lua_State* L)
		{
			T* obj = static_cast<T*>(lua_touserdata(L, -1));
			obj->~T();
			return 0;
		}

		template<class T>
		struct garbage_collector_s
		{
			static int apply(lua_State* L)
			{
				T* obj = static_cast<T*>(lua_touserdata(L, -1));
				obj->~T();
				return 0;
			}
		};

	}
}

#endif // LUABIND_GARBAGE_COLLECTOR_HPP_INCLUDED

