// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#ifndef LUABIND_PRIMITIVES_HPP_INCLUDED
#define LUABIND_PRIMITIVES_HPP_INCLUDED

	// std::reference_wrapper...
#include <type_traits>  // std::true_type...
#include <cstring>

namespace luabind {
	namespace detail {

		template<class T>
		struct type_ {};

		struct ltstr
		{
			bool operator()(const char* s1, const char* s2) const { return std::strcmp(s1, s2) < 0; }
		};

		template<int N>
		struct aligned
		{
			char storage[N];
		};

		// returns the offset added to a Derived* when cast to a Base*
		template<class Derived, class Base>
		ptrdiff_t ptr_offset(type_<Derived>, type_<Base>)
		{
			aligned<sizeof(Derived)> obj;
			Derived* ptr = reinterpret_cast<Derived*>(&obj);

			return ptrdiff_t(static_cast<char*>(static_cast<void*>(static_cast<Base*>(ptr)))
				- static_cast<char*>(static_cast<void*>(ptr)));
		}

	}
}

#endif // LUABIND_PRIMITIVES_HPP_INCLUDED

