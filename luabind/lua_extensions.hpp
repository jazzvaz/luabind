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
	// Based on https://github.com/keplerproject/lua-compat-5.2
	// Converts any Lua value at the given index to a C string in a reasonable format.
	// The resulting string is pushed onto the stack and also returned by the function.
	// If len is not NULL, the function also sets *len with the string length.
	// Does NOT support __tostring metamethod.
	LUABIND_API const char* lua52L_tolstring(lua_State* L, int idx, size_t* len);
} // namespace luabind

#endif
