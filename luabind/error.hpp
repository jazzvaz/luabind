// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/prefix.hpp>
#include <stdexcept>
#include <luabind/config.hpp>
#include <luabind/error_callback_fun.hpp>
#include <luabind/lua_state_fwd.hpp>
#include <string>

#ifndef LUABIND_NO_EXCEPTIONS
#include <luabind/typeid.hpp>
#endif

namespace luabind
{
#ifndef LUABIND_NO_EXCEPTIONS
    // this exception usually means that the lua function you called
    // from C++ failed with an error code. You will have to
    // read the error code from the top of the lua stack
    // note that luabind::string's copy constructor
    // may throw, if the copy constructor of an exception that is
    // being thrown throws another exception, terminate will be called
    // and the entire application is killed.
    class LUABIND_API error : public std::exception
    {
    public:
        explicit error(lua_State* L);
        virtual const char* what() const throw();

    private:
        luabind::string m_message;
    };

    // if an object_cast<>() fails, this is thrown
    // it is also thrown if the return value of
    // a lua function cannot be converted
    class LUABIND_API cast_failed : public std::exception
    {
    public:
        cast_failed(lua_State* L, type_id const& i) :
            m_L(L),
            m_info(i)
        {}
        lua_State* state() const throw() { return m_L; }
        type_id info() const throw() { return m_info; }
        virtual const char* what() const throw() { return "unable to make cast"; }

    private:
        lua_State* m_L;
        type_id m_info;
    };

#else
    LUABIND_API void set_call_error_break(bool enable);
    LUABIND_API bool get_call_error_break();
    LUABIND_API void set_cast_error_break(bool enable);
    LUABIND_API bool get_cast_error_break();

    LUABIND_API void set_error_callback(error_callback_fun e);
    LUABIND_API void set_cast_failed_callback(cast_failed_callback_fun c);
    LUABIND_API error_callback_fun get_error_callback();
    LUABIND_API cast_failed_callback_fun get_cast_failed_callback();
#endif

    LUABIND_API void set_pcall_callback(pcall_callback_fun e);
    LUABIND_API pcall_callback_fun get_pcall_callback();

    namespace detail
    {
        extern bool permissive;
    } // namespace detail

    // This will enable the following features:
    // - native converter from number to <luabind::string> and <char const*>
    // - function calls with missing arguments are not treated as errors
    // - return value of lua function is converted to cpp without converter match check
    LUABIND_API void set_permissive_mode(bool enable);
    LUABIND_API bool get_permissive_mode();

    // thrown when trying to use unregistered class or call nonexistent function
    class LUABIND_API unresolved_name : public std::runtime_error
    {
    public:
        unresolved_name(const char* desc, const char* name);
    };
} // namespace luabind
