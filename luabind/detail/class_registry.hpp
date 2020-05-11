// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <map>

#include <luabind/config.hpp>
#include <luabind/open.hpp>
#include <luabind/typeid.hpp>

namespace luabind {
	namespace detail {

		class class_rep;

		extern char class_registry_tag;

		struct LUABIND_API class_registry
		{
			class_registry(lua_State* L);

			static class_registry* get_registry(lua_State* L);

			int cpp_instance() const { return m_instance_metatable; }
			int cpp_class() const { return m_cpp_class_metatable; }

			int lua_instance() const { return m_instance_metatable; }
			int lua_class() const { return m_lua_class_metatable; }
			int lua_function() const { return m_lua_function_metatable; }

			void add_class(type_id const& info, class_rep* crep);

			class_rep* find_class(type_id const& info) const;

			luabind::map<type_id, class_rep*> const& get_classes() const
			{
				return m_classes;
			}

		private:

			luabind::map<type_id, class_rep*> m_classes;

			// this is a lua reference that points to the lua table
			// that is to be used as meta table for all C++ class 
			// instances. It is a kind of v-table.
			int m_instance_metatable;

			// this is a lua reference to the metatable to be used
			// for all classes defined in C++.
			int m_cpp_class_metatable;

			// this is a lua reference to the metatable to be used
			// for all classes defined in lua
			int m_lua_class_metatable;

			// this metatable only contains a destructor
			// for luabind::Detail::free_functions::function_rep
			int m_lua_function_metatable;

		};

	}
}
