// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#ifndef WEAK_REF_040402_HPP
#define WEAK_REF_040402_HPP

#include <luabind/config.hpp>

#include <luabind/lua_state_fwd.hpp>

namespace luabind {

	class LUABIND_API weak_ref
	{
	public:
		weak_ref();
		weak_ref(lua_State* main, lua_State* L, int index);
		weak_ref(weak_ref const&);
		~weak_ref();

		weak_ref& operator=(weak_ref const&);

		void swap(weak_ref&);

		// returns a unique id that no
		// other weak ref will return
		int id() const;

		lua_State* state() const;
		void get(lua_State* L) const;

	private:
		struct impl;
		impl* m_impl;
	};

} // namespace luabind

#endif // WEAK_REF_040402_HPP

