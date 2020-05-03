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

#ifndef LUA_EXTENSIONS_HPP_INCLUDED
#define LUA_EXTENSIONS_HPP_INCLUDED

#include <luabind/config.hpp>

namespace luabind
{
	// Port of Lua5.2 luaL_tolstring()
	// Based on http://www.lua.org/source/5.2/lauxlib.c.html#luaL_tolstring
	// Converts any Lua value at the given index to a C string in a reasonable format.
	// The resulting string is pushed onto the stack and also returned by the function.
	// If len is not NULL, the function also sets *len with the string length.
	// If the value has a metatable with a "__tostring" field, then luaL_tolstring calls
	// the corresponding metamethod with the value as argument, and uses the result of
	// the call as its result.
	LUABIND_API const char* lua52L_tolstring(lua_State* L, int idx, size_t* len);

	namespace detail
	{
		LUABIND_API const char* luaL_tolstring_diag(lua_State* L, int idx, size_t* len);
	} // namespace detail
} // namespace luabind

#endif
