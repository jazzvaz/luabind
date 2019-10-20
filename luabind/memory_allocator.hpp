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

#ifndef LUABIND_MEMORY_ALLOCATOR_HPP_INCLUDED
#define LUABIND_MEMORY_ALLOCATOR_HPP_INCLUDED

#include <luabind/memory.hpp>

namespace luabind
{
	template <class T>
	class memory_allocator
	{
	private:
		using self_type = memory_allocator<T>;

	public:
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using const_pointer = T const*;
		using reference = T&;
		using const_reference = T const&;
		using value_type = T;
		
		memory_allocator() {}
		memory_allocator(memory_allocator<T> const&) {}
		template<class T2>
		memory_allocator(memory_allocator<T2> const&) {}

		template<class T2>
		memory_allocator<T>& operator=(memory_allocator<T2> const&)
		{
			return *this;
		}

		pointer address(reference value) const
		{
			return &value;
		}

		const_pointer address(const_reference value) const
		{
			return &value;
		}

		pointer allocate(size_type n, void const* p = nullptr) const
		{
			pointer result = (pointer)detail::call_allocator(p, n * sizeof(T));
			if (!n)
				result = (pointer)detail::call_allocator(p, sizeof(T));
			return result;
		}

		void deallocate(pointer p, size_type) const
		{
			detail::call_allocator(p, 0);
		}

		void deallocate(void* p, size_type) const
		{
			detail::call_allocator(p, 0);
		}

		void construct(pointer p, T const& value)
		{
			new(p) T(value);
		}

		void destroy(pointer p)
		{
			p->~T();
		}

		size_type max_size() const
		{
			size_type count = (size_type)(-1) / sizeof(T);
			if (count)
				return count;
			return 1;
		}
	};
	
	template <class T1, class T2>
	bool operator==(memory_allocator<T1> const&, memory_allocator<T2> const&)
	{
		return true;
	}

	template <class T1, class T2>
	bool operator!=(memory_allocator<T1> const&, memory_allocator<T2> const&)
	{
		return false;
	}
}

#endif // LUABIND_MEMORY_ALLOCATOR_HPP_INCLUDED
