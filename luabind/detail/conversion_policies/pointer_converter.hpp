// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#ifndef LUABIND_POINTER_CONVERTER_HPP_INCLUDED
#define LUABIND_POINTER_CONVERTER_HPP_INCLUDED

#include <type_traits>
#include <luabind/lua_include.hpp>
#include <luabind/detail/make_instance.hpp>
#include <luabind/back_reference.hpp>

namespace luabind {

	/*
		Todo: Remove code duplication
	*/

	namespace detail {

		struct pointer_converter
		{
			using type      = pointer_converter;
			using is_native = std::false_type;

			pointer_converter()
				: result(0)
			{}

			void* result;

			enum { consumed_args = 1 };

			template<class T>
			static void to_lua(lua_State* L, T* ptr)
			{
				if(ptr == 0)
				{
					lua_pushnil(L);
					return;
				}

				if(luabind::get_back_reference(L, ptr))
					return;

				make_pointer_instance(L, ptr);
			}

			template<class T>
			T* to_cpp(lua_State*, by_pointer<T>, int /*index*/)
			{
				return static_cast<T*>(result);
			}

			template<class T>
			int match(lua_State* L, by_pointer<T>, int index)
			{
				if(lua_isnil(L, index)) return 0;
				object_rep* obj = get_instance(L, index);
				if(obj == 0) return no_match;

				if(obj->is_const())
					return no_match;

				std::pair<void*, int> s = obj->get_instance(registered_class<T>::id);
				result = s.first;
				return s.second;
			}

			template<class T>
			void converter_postcall(lua_State*, by_pointer<T>, int /*index*/)
			{}
		};

		struct const_pointer_converter
		{
			using type      = const_pointer_converter;
			using is_native = std::false_type;

			enum { consumed_args = 1 };

			const_pointer_converter()
				: result(0)
			{}

			void* result;

			template<class T>
			void to_lua(lua_State* L, const T* ptr)
			{
				if(ptr == 0)
				{
					lua_pushnil(L);
					return;
				}

				if(luabind::get_back_reference(L, ptr))
					return;

				make_pointer_instance(L, ptr);
			}

			template<class T>
			T const* to_cpp(lua_State*, by_const_pointer<T>, int)
			{
				return static_cast<T const*>(result);
			}

			template<class T>
			int match(lua_State* L, by_const_pointer<T>, int index)
			{
				if(lua_isnil(L, index)) return 0;
				object_rep* obj = get_instance(L, index);
				if(obj == 0) return no_match; // if the type is not one of our own registered types, classify it as a non-match
				std::pair<void*, int> s = obj->get_instance(registered_class<T>::id);
				if(s.second >= 0 && !obj->is_const())
					s.second += 10;
				result = s.first;
				return s.second;
			}

			template<class T>
			void converter_postcall(lua_State*, T, int) {}
		};

	}

}

#endif

