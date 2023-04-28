// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2004 The Luabind Authors

#pragma once

#include <luabind/prefix.hpp>
#include <luabind/config.hpp>
#include <luabind/object.hpp>
#include <luabind/lua_state_fwd.hpp>
#include <memory>

namespace luabind
{
    struct scope;
} // namespace luabind

namespace luabind::detail
{
    struct LUABIND_API registration
    {
    public:
        virtual ~registration();

    protected:
        virtual void register_(lua_State*, bool default_scope = false) const = 0;

    private:
        friend struct ::luabind::scope;
        registration* m_next = nullptr; // XXX: convert to unique_ptr?
    };
} // namespace luabind::detail

namespace luabind
{
    struct LUABIND_API scope
    {
        scope();
        explicit scope(luabind::unique_ptr<detail::registration> reg);
        scope(scope const& other_);
        ~scope();

        scope& operator=(scope const& other_);
        scope& operator,(scope s);

        void register_(lua_State* L, bool default_scope = false) const;

    private:
        detail::registration* m_chain; // XXX: convert to unique_ptr?
    };

    class LUABIND_API namespace_ : public scope
    {
    public:
        explicit namespace_(char const* name);
        namespace_& operator[](scope s);

    private:
        struct registration_;
        registration_* m_registration;
    };

    class LUABIND_API module_
    {
    public:
        module_(lua_State* L_, char const* name);
        module_(object const& table);
        void operator[](scope s);

    private:
        object m_table;
    };

    inline module_ module(object const& table)
    {
        return module_(table);
    }

    inline module_ module(lua_State* L, char const* name = nullptr)
    {
        return module_(L, name);
    }

} // namespace luabind
