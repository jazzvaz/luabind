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

#define LUABIND_BUILDING

#include <luabind/lua_include.hpp>
#include <luabind/lua_extensions.hpp>
#include <luabind/class_info.hpp>

namespace luabind
{
	LUABIND_API const char* lua52L_tolstring(lua_State* L, int idx, size_t* len)
	{
		if (!luaL_callmeta(L, idx, "__tostring"))
		{
			switch (lua_type(L, idx))
			{
			case LUA_TSTRING:
			case LUA_TNUMBER:
				lua_pushvalue(L, idx);
				break;
			case LUA_TBOOLEAN:
				if (lua_toboolean(L, idx))
					lua_pushliteral(L, "true");
				else
					lua_pushliteral(L, "false");
				break;
			case LUA_TNIL:
				lua_pushliteral(L, "nil");
				break;
			default:
				lua_pushfstring(L, "%s: %p", luaL_typename(L, idx), lua_topointer(L, idx));
				break;
			}
		}
		return lua_tolstring(L, -1, len);
	}
	
	namespace detail
	{
		LUABIND_API const char* luaL_tolstring_diag(lua_State* L, int idx, size_t* len)
		{
			if (!luaL_callmeta(L, idx, "__tostring"))
			{
				int t = lua_type(L, idx);
				switch (t)
				{
				case LUA_TNIL:
					lua_pushliteral(L, "nil");
					break;
				case LUA_TSTRING:
					lua_pushliteral(L, "'");
					lua_pushvalue(L, idx);
					lua_pushliteral(L, "'");
					lua_concat(L, 3);
					break;
				case LUA_TNUMBER:
					lua_pushvalue(L, idx);
					break;
				case LUA_TBOOLEAN:
					if (lua_toboolean(L, idx))
						lua_pushliteral(L, "true");
					else
						lua_pushliteral(L, "false");
					break;
				case LUA_TUSERDATA:
				{
					// Print luabind::object class name
					auto obj = argument(from_stack(L, idx));
					lua_pushstring(L, get_class_info(obj).name.c_str());
					lua_pushliteral(L, "&");
					lua_concat(L, 2);
					break;
				}
				default:
					lua_pushfstring(L, "%s: %p", lua_typename(L, t), lua_topointer(L, idx));
					break;
				}
			}
			return lua_tolstring(L, -1, len);
		}
	} // namespace detail
} // namespace luabind
