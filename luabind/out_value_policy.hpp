// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>        // for find_conversion_policy, etc
#include <luabind/detail/decorate_type.hpp> // for decorated_type
#include <luabind/detail/primitives.hpp>    // for by_pointer, by_reference, etc
#include <luabind/detail/type_traits.hpp>   // for is_nonconst_pointer, is_nonconst_reference, etc
#include <new>                              // for operator new

namespace luabind::detail
{
    template <int N>
    struct char_array
    {
        char storage[N];
    };

    template <class U>
    char_array<sizeof(identity_t<U>)> indirect_sizeof_test(by_reference<U>);

    template <class U>
    char_array<sizeof(identity_t<U>)> indirect_sizeof_test(by_const_reference<U>);

    template <class U>
    char_array<sizeof(identity_t<U>)> indirect_sizeof_test(by_pointer<U>);

    template <class U>
    char_array<sizeof(identity_t<U>)> indirect_sizeof_test(by_const_pointer<U>);

    template <class U>
    char_array<sizeof(identity_t<U>)> indirect_sizeof_test(by_value<U>);

    template <typename T>
    constexpr int indirect_sizeof = sizeof(indirect_sizeof_test(decorate_type_t<T>()));

    namespace out_value_detail
    {
        template <int Size>
        struct temporary_storage_size
        {
            template <typename T, typename... Args>
            void construct(Args&&... args)
            { new (&m_storage) T(std::forward<Args>(args)...); }

            template <typename T>
            T& get() { return *reinterpret_cast<T*>(&m_storage); }

            template <typename T>
            const T& get() const { return *reinterpret_cast<T*>(&m_storage); }

            template <typename T>
            void destroy() { get<T>().~T(); }

            std::aligned_storage_t<Size> m_storage;
        };

        template <typename T>
        struct temporary_storage_type
        {
            template <typename... Args>
            void construct(Args&&... args)
            { new (&m_storage) T(std::forward<Args>(args)...); }

            T& get() { return *reinterpret_cast<T*>(&m_storage); }

            const T& get() const { return *reinterpret_cast<T*>(&m_storage); }

            void destroy() { get().~T(); }

            std::aligned_storage_t<sizeof(T), alignof(T)> m_storage;
        };
    } // namespace luabind::detail::out_value_detail

    // See note in out_value_policy about why we're not templating
    // for the parameter type.
    template <typename T, class Policies = no_policies>
    struct out_value_converter
    {
        static constexpr int consumed_args = 1;

        T& to_cpp(lua_State* L, by_reference<T>, int index)
        {
            storage_.construct(converter_.to_cpp(L, decorate_type_t<T>(), index));
            return storage_.get();
        }

        int match(lua_State* L, by_reference<T>, int index)
        {
            return converter_.match(L, decorate_type_t<T>(), index);
        }

        void converter_postcall(lua_State* L, by_reference<T>, int)
        {
            converter_.to_lua(L, storage_.get());
            storage_.destroy();
        }

        T* to_cpp(lua_State* L, by_pointer<T>, int index)
        {
            storage_.construct(converter_.to_cpp(L, decorate_type_t<T>(), index));
            return &storage_.get();
        }

        int match(lua_State* L, by_pointer<T>, int index)
        {
            return converter_.match(L, decorate_type_t<T>(), index);
        }

        void converter_postcall(lua_State* L, by_pointer<T>, int)
        {
            //specialized_converter_policy_n<2, Policies, T, cpp_to_lua> converter;
            converter_.to_lua(L, storage_.get());
            storage_.destroy();
        }

    private:
        specialized_converter_policy_n<1, Policies, T, lua_to_cpp> converter_;
        out_value_detail::temporary_storage_type<T> storage_;
    };

    template <class Policies = no_policies>
    struct out_value_policy
    {
        template <class T, class Direction>
        struct specialize
        {
            static_assert(std::is_same_v< Direction, lua_to_cpp>,
                "Out value policy can only convert from lua to cpp");
            static_assert(is_nonconst_reference_v<T> || is_nonconst_pointer_v<T>,
                "Out value policy only accepts non const references or pointers");
            // Note to myself:
            // Using the size and template members instead of a policy templated for the type seems
            // to be done to tame template bloat. Need to check if this is worth is.
            using base_type = std::remove_pointer_t<std::remove_reference_t<T>>;
            using type = out_value_converter<base_type, Policies>;
        };
    };

    template <int Size, class Policies = no_policies>
    struct pure_out_value_converter
    {
        static constexpr int consumed_args = 0;

        template <class T>
        T& to_cpp(lua_State*, by_reference<T>, int)
        {
            storage_.decltype(storage_)::template construct<T>();
            return storage_.template get<T>();
        }

        template <class T>
        static int match(lua_State*, by_reference<T>, int)
        { return 0; }

        template <class T>
        void converter_postcall(lua_State* L, by_reference<T>, int)
        {
            specialized_converter_policy_n<1, Policies, T, cpp_to_lua> converter;
            converter.to_lua(L, storage_.template get<T>());
            storage_.template destroy<T>();
        }

        template <class T>
        T* to_cpp(lua_State*, by_pointer<T>, int)
        {
            storage_.decltype(storage_)::template construct<T>();
            return &storage_.template get<T>();
        }

        template <class T>
        static int match(lua_State*, by_pointer<T>, int)
        { return 0; }

        template <class T>
        void converter_postcall(lua_State* L, by_pointer<T>, int)
        {
            specialized_converter_policy_n<1, Policies, T, cpp_to_lua> converter;
            converter.to_lua(L, storage_.template get<T>());
            storage_.template destroy<T>();
        }

    private:
        out_value_detail::temporary_storage_size<Size> storage_;
    };

    template <class Policies = no_policies>
    struct pure_out_value_policy
    {
        template <class T, class Direction>
        struct specialize
        {
            static_assert(std::is_same_v<Direction, lua_to_cpp>,
                "Pure out value policy can only convert from lua to cpp");
            static_assert(is_nonconst_reference_v<T> || is_nonconst_pointer_v<T>,
                "Pure out value policy only accepts non const references or pointers");
            using type = pure_out_value_converter<indirect_sizeof<T>, Policies>;
        };
    };
} // namespace luabind::detail

namespace luabind::policy
{
    template <uint32_t N>
    using out_value = converter_injector<N, detail::out_value_policy<>>;

    template <uint32_t N>
    using pure_out_value = converter_injector<N, detail::pure_out_value_policy<>>;
} // namespace luabind::policy
