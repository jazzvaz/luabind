// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <typeinfo>
#include <cstdlib>
#include <luabind/types.hpp>
#include <luabind/detail/type_traits.hpp>

#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
# define LUABIND_DEMANGLE_TYPENAMES
# include <cxxabi.h>
#endif

namespace luabind
{
	class type_id
	{
	public:
		type_id() :
			id(&typeid(null_type))
		{}

		type_id(std::type_info const& id) :
			id(&id)
		{}

		bool operator!=(type_id const& other) const
		{ return *id != *other.id; }

		bool operator==(type_id const& other) const
		{ return *id == *other.id; }

		bool operator<(type_id const& other) const
		{ return id->before(*other.id); }

		size_t hash_code() const
		{ return id->hash_code(); }

		luabind::string name() const
		{
#ifdef LUABIND_DEMANGLE_TYPENAMES
			int status;
			char* buf = abi::__cxa_demangle(id->name(), 0, 0, &status);
			if (buf)
			{
				luabind::string name(buf);
				std::free(buf);
				return name;
			}
#endif
			return id->name();
		}

	private:
		std::type_info const* id;
	};
} // namespace luabind
