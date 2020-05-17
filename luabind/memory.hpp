// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2019 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/types.hpp>

namespace luabind
{
#ifdef LUABIND_CUSTOM_ALLOCATOR
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
#endif

	template <typename T, typename... Args>
	T* luabind_new(Args&&... args)
	{
#ifdef LUABIND_CUSTOM_ALLOCATOR
		auto result = reinterpret_cast<T*>(detail::call_allocator(nullptr, sizeof(T)));
		return new (result) T(std::forward<Args>(args)...);
#else
		return new T(std::forward<Args>(args)...);
#endif
	}

#ifdef LUABIND_CUSTOM_ALLOCATOR
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
#endif

	template <typename T>
	void luabind_delete(T*& pointer)
	{
#ifdef LUABIND_CUSTOM_ALLOCATOR
		if (!pointer)
			return;
		detail::delete_helper<T, std::is_polymorphic_v<T>>::apply(pointer);
#else
		delete pointer;
#endif
	}

#ifdef LUABIND_CUSTOM_ALLOCATOR
	template <typename T>
	struct luabind_deleter
	{
		void operator()(T* ptr) const
		{
			luabind_delete(ptr);
		}
	};
#else
	template <typename T>
	using luabind_deleter = std::default_delete<T>;
#endif
}
