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

#ifndef LUABIND_TYPES_HPP_INCLUDED
#define LUABIND_TYPES_HPP_INCLUDED

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <queue>
#include <luabind/memory_allocator.hpp>

namespace luabind
{
	template <typename T>
	using unique_ptr = std::unique_ptr<T, luabind_deleter<T>>;
	using string = std::basic_string<char, std::char_traits<char>, memory_allocator<char>>;
	using stringstream = std::basic_stringstream<char, std::char_traits<char>, memory_allocator<char>>;
	template <typename T>
	using vector = std::vector<T, memory_allocator<T>>;
	template <typename T>
	using list = std::list<T, memory_allocator<T>>;
	template <typename K, class P = std::less<K>>
	using set = std::set<K, P, memory_allocator<K>>;
	template <typename K, class P = std::less<K>>
	using multiset = std::multiset<K, P, memory_allocator<K>>;
	template <typename K, class V, class P = std::less<K>>
	using map = std::map<K, V, P, memory_allocator<std::pair<const K, V>>>;
	template <typename K, class V, class P = std::less<K>>
	using multimap = std::multimap<K, V, P, memory_allocator<std::pair<const K, V>>>;
	template <typename T>
	using dequeue = std::deque<T, memory_allocator<T>>;
	template <typename T>
	using queue = std::queue<T, dequeue<T>>;
}

#endif // LUABIND_TYPES_HPP_INCLUDED
