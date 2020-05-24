// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <tuple>

#include <luabind/nil.hpp>
#include <luabind/handle.hpp>
#include <luabind/from_stack.hpp>
#include <luabind/detail/stack_utils.hpp>
#include <luabind/lua_stack.hpp>
#include <luabind/typeid.hpp>
#include <luabind/detail/crtp_iterator.hpp>
#include <luabind/lua_proxy_interface.hpp>
#include <luabind/lua_index_proxy.hpp>
#include <luabind/lua_iterator_proxy.hpp>
#include <luabind/detail/class_rep.hpp>

namespace luabind::adl
{
	// An object holds a reference to a Lua value residing
	// in the registry.
	class object : public lua_proxy_interface<object>
	{
	public:
		object() = default;

		explicit object(handle const& other) :
			m_handle(other)
		{}

		explicit object(from_stack const& stack_reference) :
			m_handle(stack_reference.interpreter, stack_reference.index)
		{}

		template <class T, class Policies = no_policies, int PolicyIndex = 1>
		object(lua_State* L, T&& value)
		{
			lua_stack::push<Policies, PolicyIndex>(L, std::forward<T>(value));
			detail::stack_pop pop(L, 1);
			handle(L, -1).swap(m_handle);
		}

		void push(lua_State* L) const { m_handle.push(L); }
		lua_State* interpreter() const { return m_handle.interpreter(); }
		bool is_valid() const { return !!m_handle.interpreter(); }

		template <class T>
		index_proxy<object> operator[](T const& key) const
		{ return index_proxy<object>(*this, m_handle.interpreter(), key); }

		void swap(object& other)
		{ m_handle.swap(other.m_handle); }

	private:
		handle m_handle;
	};
} // namespace luabind::adl

namespace luabind
{
	using adl::object;

	template <>
	struct lua_proxy_traits<object>
	{
		using is_specialized = std::true_type;

		static lua_State* interpreter(object const& value)
		{ return value.interpreter(); }

		static void unwrap(lua_State* L, object const& value)
		{ value.push(L); }

		static bool check(...)
		{ return true; }
	};

	template <class R, typename PolicyList = no_policies, typename... Args>
	R call_function(luabind::object const& obj, Args&&... args)
	{
		obj.push(obj.interpreter());
		return call_pushed_function<R, PolicyList>(obj.interpreter(), std::forward<Args>(args)...);
	}

	template <class R, typename PolicyList = no_policies, typename... Args>
	R resume_function(luabind::object const& obj, Args&&... args)
	{
		obj.push(obj.interpreter());
		return resume_pushed_function<R, PolicyList>(obj.interpreter(), std::forward<Args>(args)...);
	}

	// declared in luabind/lua_index_proxy.hpp
	template <typename Next>
	adl::index_proxy<Next>::operator object()
	{
		detail::stack_pop pop(m_interpreter, 1);
		push(m_interpreter);
		return object(from_stack(m_interpreter, -1));
	}

	// declared in luabind/lua_proxy_interface.hpp
	template <typename ProxyType>
	template <typename PolicyList, typename... Args>
	object adl::lua_proxy_interface<ProxyType>::call(Args&&... args)
	{
		return call_function<object, PolicyList>(derived(), std::forward<Args>(args)...);
	}

	// declared in luabind/lua_proxy_interface.hpp
	template <typename ProxyType>
	template <typename... Args>
	object adl::lua_proxy_interface<ProxyType>::operator()(Args&&... args)
	{
		return call<no_policies>(std::forward<Args>(args)...);
	}

	// declared in luabind/lua_iterator_proxy.hpp
	template <class AccessPolicy>
	adl::iterator_proxy<AccessPolicy>::operator object()
	{
		lua_pushvalue(m_interpreter, m_key_index);
		AccessPolicy::get(m_interpreter, m_table_index);
		detail::stack_pop pop(m_interpreter, 1);
		return object(from_stack(m_interpreter, -1));
	}

	// declared in luabind/lua_iterator_proxy.hpp
	template <class AccessPolicy>
	object detail::basic_iterator<AccessPolicy>::key() const
	{ return object(m_key); }

	namespace adl
	{
		// Simple value_wrapper adaptor with the sole purpose of helping with
		// overload resolution. Use this as a function parameter type instead
		// of "object" or "argument" to restrict the parameter to Lua tables.
		template <class Base = object>
		struct table : Base
		{
			table(from_stack const& stack_reference) :
				Base(stack_reference)
			{}
		};
	} // namespace adl

	using adl::table;

	template <class Base>
	struct lua_proxy_traits<adl::table<Base>> : lua_proxy_traits<Base>
	{
		static bool check(lua_State* L, int idx)
		{
			return lua_proxy_traits<Base>::check(L, idx) && lua_istable(L, idx);
		}
	};

	inline object newtable(lua_State* L)
	{
		lua_newtable(L);
		detail::stack_pop pop(L, 1);
		return object(from_stack(L, -1));
	}

	// this could be optimized by returning a proxy
	inline object globals(lua_State* L)
	{
		lua_pushglobaltable(L);
		detail::stack_pop pop(L, 1);
		return object(from_stack(L, -1));
	}

	// this could be optimized by returning a proxy
	inline object registry(lua_State* L)
	{
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		detail::stack_pop pop(L, 1);
		return object(from_stack(L, -1));
	}

	template <class ValueWrapper, class K>
	object gettable(ValueWrapper const& table, K&& key)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(table);
		lua_proxy_traits<ValueWrapper>::unwrap(L, table);
		detail::stack_pop pop(L, 2);
		lua_stack::push(L, std::forward<K>(key));
		lua_gettable(L, -2);
		return object(from_stack(L, -1));
	}

	template <class ValueWrapper, class K, class T>
	void settable(ValueWrapper const& table, K&& key, T&& value)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(table);
		// TODO: Exception safe?
		lua_proxy_traits<ValueWrapper>::unwrap(L, table);
		detail::stack_pop pop(L, 1);
		lua_stack::push(L, std::forward<K>(key));
		lua_stack::push(L, std::forward<T>(value));
		lua_settable(L, -3);
	}

	template <class ValueWrapper, class K>
	object rawget(ValueWrapper const& table, K&& key)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(table);
		lua_proxy_traits<ValueWrapper>::unwrap(L, table);
		detail::stack_pop pop(L, 2);
		lua_stack::push(L, std::forward<K>(key));
		lua_rawget(L, -2);
		return object(from_stack(L, -1));
	}

	template <class ValueWrapper, class K, class T>
	void rawset(ValueWrapper const& table, K&& key, T&& value)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(table);
		// TODO: Exception safe?
		lua_proxy_traits<ValueWrapper>::unwrap(L, table);
		detail::stack_pop pop(L, 1);
		lua_stack::push(L, std::forward<K>(key));
		lua_stack::push(L, std::forward<T>(value));
		lua_rawset(L, -3);
	}

	template <class ValueWrapper>
	int type(ValueWrapper const& value)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(value);
		lua_proxy_traits<ValueWrapper>::unwrap(L, value);
		detail::stack_pop pop(L, 1);
		return lua_type(L, -1);
	}

	template <class ValueWrapper>
	object getmetatable(ValueWrapper const& obj)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(obj);
		lua_proxy_traits<ValueWrapper>::unwrap(L, obj);
		detail::stack_pop pop(L, 2);
		lua_getmetatable(L, -1);
		return object(from_stack(L, -1));
	}

	template <class ValueWrapper1, class ValueWrapper2>
	void setmetatable(ValueWrapper1 const& obj, ValueWrapper2 const& metatable)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper1>::interpreter(obj);
		lua_proxy_traits<ValueWrapper1>::unwrap(L, obj);
		detail::stack_pop pop(L, 1);
		lua_proxy_traits<ValueWrapper2>::unwrap(L, metatable);
		lua_setmetatable(L, -2);
	}

	template <class ValueWrapper>
	std::tuple<const char*, object> getupvalue(ValueWrapper const& value, int index)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper>::interpreter(value);
		lua_proxy_traits<ValueWrapper>::unwrap(L, value);
		detail::stack_pop pop(L, 2);
		const char* name = lua_getupvalue(L, -1, index);
		return {name, object(from_stack(L, -1))};
	}

	template <class ValueWrapper1, class ValueWrapper2>
	void setupvalue(ValueWrapper1 const& function, int index, ValueWrapper2 const& value)
	{
		lua_State* L = lua_proxy_traits<ValueWrapper1>::interpreter(function);
		lua_proxy_traits<ValueWrapper1>::unwrap(L, function);
		detail::stack_pop pop(L, 1);
		lua_proxy_traits<ValueWrapper2>::unwrap(L, value);
		lua_setupvalue(L, -2, index);
	}

	template <class GetValueWrapper, class SetValueWrapper = null_type>
	object property(GetValueWrapper const& get, SetValueWrapper const& set = SetValueWrapper())
	{
		lua_State* L = lua_proxy_traits<GetValueWrapper>::interpreter(get);
		lua_proxy_traits<GetValueWrapper>::unwrap(L, get);
		if constexpr (!is_null_type_v<SetValueWrapper>)
			lua_proxy_traits<SetValueWrapper>::unwrap(L, set);
		else
			lua_pushnil(L);		
		lua_pushcclosure(L, &detail::property_tag, 2);
		detail::stack_pop pop(L, 1);
		return object(from_stack(L, -1));
	}
} // namespace luabind

#include <luabind/detail/conversion_policies/conversion_policies.hpp>
