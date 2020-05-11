// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#define LUABIND_BUILDING

#include <luabind/error.hpp>
#ifndef LUA_INCLUDE_HPP_INCLUDED
#include <luabind/lua_include.hpp>
#endif


namespace luabind {

#ifndef LUABIND_NO_EXCEPTIONS	
	error::error(lua_State* L)
	{
		const char* message = lua_tostring(L, -1);

		if(message)
		{
			m_message = message;
		}

		lua_pop(L, 1);
	}


	const char* error::what() const throw()
	{
		return m_message.c_str();
	}
#endif
	namespace
	{
		pcall_callback_fun pcall_callback = 0;
#ifdef LUABIND_NO_EXCEPTIONS
		error_callback_fun error_callback = 0;
		cast_failed_callback_fun cast_failed_callback = 0;
#endif
	}


#ifdef LUABIND_NO_EXCEPTIONS

	static bool break_on_call_error = true;
	static bool break_on_cast_error = true;

	LUABIND_API void set_call_error_break(bool enable)
	{
		break_on_call_error = enable;
	}

	LUABIND_API bool get_call_error_break()
	{
		return break_on_call_error;
	}
	
	LUABIND_API void set_cast_error_break(bool enable)
	{
		break_on_cast_error = enable;
	}

	LUABIND_API bool get_cast_error_break()
	{
		return break_on_cast_error;
	}

	void set_error_callback(error_callback_fun e)
	{
		error_callback = e;
	}

	void set_cast_failed_callback(cast_failed_callback_fun c)
	{
		cast_failed_callback = c;
	}

	error_callback_fun get_error_callback()
	{
		return error_callback;
	}

	cast_failed_callback_fun get_cast_failed_callback()
	{
		return cast_failed_callback;
	}

#endif

	void set_pcall_callback(pcall_callback_fun e)
	{
		pcall_callback = e;
	}

	pcall_callback_fun get_pcall_callback()
	{
		return pcall_callback;
	}

	unresolved_name::unresolved_name(const char* desc, const char* name) :
		std::runtime_error((luabind::string(desc) + ": " + name).c_str())
	{}

	namespace detail
	{
		bool permissive = false;
	} // namespace detail

	void set_permissive_mode(bool enable)
	{ detail::permissive = enable; }

	bool get_permissive_mode()
	{ return detail::permissive; }
}
