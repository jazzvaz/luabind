// Copyright (c) 2003 Daniel Wallin, Arvid Norberg, and contributors

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef LUABIND_MEMORY_HPP_INCLUDED
#define LUABIND_MEMORY_HPP_INCLUDED

#include <luabind/config.hpp>
#include <luabind/types.hpp>

namespace luabind
{
	using allocator_func = void* (__cdecl*)(void* context, void const* ptr, size_t size);
	extern LUABIND_API allocator_func allocator;
	extern LUABIND_API void* allocator_context;

	namespace detail
	{
		inline void* call_allocator(void const* ptr, size_t size)
		{
			return allocator(allocator_context, ptr, size);
		}
	}

	template <typename T, typename... Args>
	T* luabind_new(Args&&... args)
	{
		auto result = reinterpret_cast<T*>(detail::call_allocator(nullptr, sizeof(T)));
		return new (result) T(std::forward<Args>(args)...);
	}

	namespace detail
	{
		template <typename T>
		void delete_helper2(T*& pointer, void* top_pointer)
		{
			pointer->~T();
			call_allocator(top_pointer, 0);
			pointer = nullptr;
		}

		template <typename T, bool polymorphic>
		struct delete_helper
		{
			static void apply(T*& pointer)
			{
				delete_helper2(pointer, dynamic_cast<void*>(pointer));
			}
		};

		template <typename T>
		struct delete_helper<T, false>
		{
			static void apply(T*& pointer)
			{
				delete_helper2(pointer, pointer);
			}
		};
	}

	template <typename T>
	void luabind_delete(T*& pointer)
	{
		if (!pointer)
			return;
		detail::delete_helper<T, std::is_polymorphic<T>::value>::apply(pointer);
	}

	template <typename T>
	struct luabind_deleter
	{
		void operator()(T* ptr) const
		{
			luabind_delete(ptr);
		}
	};
}

#endif // LUABIND_MEMORY_HPP_INCLUDED
