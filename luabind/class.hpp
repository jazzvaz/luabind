// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

/*
    ISSUES:
    ------------------------------------------------------

    * solved for member functions, not application operator *
    if we have a base class that defines a function a derived class must be able to
    override that function (not just overload). Right now we just add the other overload
    to the overloads list and will probably get an ambiguity. If we want to support this
    each method_rep must include a vector of type_info pointers for each parameter.
    Operators do not have this problem, since operators always have to have
    it's own type as one of the arguments, no ambiguity can occur. Application
    operator, on the other hand, would have this problem.
    Properties cannot be overloaded, so they should always be overridden.
    If this is to work for application operator, we really need to specify if an application
    operator is const or not.

    If one class registers two functions with the same name and the same
    signature, there's currently no error. The last registered function will
    be the one that's used.
    How do we know which class registered the function? If the function was
    defined by the base class, it is a legal operation, to override it.
    we cannot look at the pointer offset, since it always will be zero for one of the bases.



    TODO:
    ------------------------------------------------------

    finish smart pointer support
        * the adopt policy should not be able to adopt pointers to held_types. This
        must be prohibited.
        * name_of_type must recognize holder_types and not return "custom"

    document custom policies, custom converters

    store the instance object for policies.

    support the __concat metamethod. This is a bit tricky, since it cannot be
    treated as a normal operator. It is a binary operator but we want to use the
    __tostring implementation for both arguments.

*/

#include <luabind/prefix.hpp>
#include <luabind/config.hpp>

#include <string>
#include <map>
#include <vector>
#include <cassert>

#include <luabind/config.hpp>
#include <luabind/scope.hpp>
#include <luabind/back_reference.hpp>
#include <luabind/function.hpp>    // -> object.hpp
#include <luabind/dependency_policy.hpp>
#include <luabind/detail/constructor.hpp>    // -> object.hpp
#include <luabind/detail/primitives.hpp>
#include <luabind/detail/property.hpp>
#include <luabind/detail/type_traits.hpp>
#include <luabind/detail/class_rep.hpp>
#include <luabind/detail/object_rep.hpp>
#include <luabind/detail/call.hpp>
#include <luabind/detail/call_member.hpp>
#include <luabind/detail/enum_maker.hpp>
#include <luabind/detail/operator_id.hpp>
#include <luabind/detail/inheritance.hpp>
#include <luabind/no_dependency.hpp>
#include <luabind/typeid.hpp>
#include <luabind/detail/meta.hpp>

// to remove the 'this' used in initialization list-warning
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4355)
#endif

namespace luabind
{
    namespace detail
    {
        template <class Derived>
        struct operator_;
    } // namespace detail

    template <typename... BaseClasses>
    struct bases {};

    using no_bases = bases<>;
    using default_holder = null_type;

    namespace detail
    {
        template <typename T>
        struct make_bases
        {
            using type = bases<T>;
        };

        template <typename... Bases>
        struct make_bases<bases<Bases...>>
        {
            using type = bases<Bases...>;
        };
    } // namespace detail

    template <typename T>
    using make_bases = typename detail::make_bases<T>::type;

    template <typename... Args>
    struct constructor {};

    // XXX: add to doc
    // helpers for overloaded methods
    template <typename Class, typename Ret, typename... Args>
    auto use_nonconst(Ret(Class::* fn)(Args...)) -> decltype(fn)
    {
        return fn;
    }

    template <typename Class, typename Ret, typename... Args>
    auto use_const(Ret(Class::* fn)(Args...) const) -> decltype(fn)
    {
        return fn;
    }

    template <typename Class, typename Ret, typename... Args>
    auto use_auto(Ret(Class::* fn)(Args...) const) -> decltype(fn)
    {
        return fn;
    }

    template <typename Class, typename Ret, typename... Args>
    auto use_auto(Ret(Class::* fn)(Args...)) -> decltype(fn)
    {
        return fn;
    }

    // TODO: Could specialize for certain base classes to make the interface "type safe".
    template <typename T, typename BaseOrBases = no_bases, typename Holder = null_type, typename Wrapper = null_type>
    struct class_;

    // This function will only be invoked if the user hasn't defined a correct overload
    template <typename SmartPtr, bool Condition = false>
    SmartPtr* get_const_holder(SmartPtr*)
    {
        static_assert(Condition, "Define a get const holder function for your smart ptr");
        return nullptr;
    }

    template <class T>
    std::shared_ptr<T const>* get_const_holder(std::shared_ptr<T>*)
    {
        return nullptr;
    }
}

namespace luabind::detail
{
    // prints the types of the values on the stack, in the
    // range [start_index, lua_gettop()]
    LUABIND_API luabind::string stack_content_by_name(lua_State* L, int start_index);

    struct LUABIND_API create_class
    {
        static int stage1(lua_State* L);
        static int stage2(lua_State* L);
    };

    template <class T>
    struct static_scope
    {
        static_scope(T& self_) :
            self(self_)
        {}

        T& operator[](scope s) const
        {
            self.add_inner_scope(s);
            return self;
        }

    private:
        template <class U>
        void operator,(U const&) const;
        void operator=(static_scope const&);

        T& self;
    };

    struct class_registration;

    struct LUABIND_API class_base : scope
    {
    public:
        class_base(char const* name);

        void init(type_id const& type, class_id id, type_id const& w_type, class_id w_id);
        const char* name() const;
        void add_base(type_id const& base);
        void add_member(registration* member);
        void add_default_member(registration* member);
        void add_static_constant(const char* name, int val);
        void add_inner_scope(scope& s);
        void add_cast(class_id src, class_id target, cast_function cast);

    private:
        class_registration* m_registration;
    };

    // MSVC complains about member being sensitive to alignment (C4121)
    // when F is a pointer to member of a class with virtual bases.
#ifdef _MSC_VER
# pragma pack(push)
# pragma pack(16)
#endif
    template <class Class, class F, class Policies>
    struct memfun_registration : registration
    {
        memfun_registration(char const* name, F f) :
            name(name),
            f(f)
        {}

        void register_(lua_State* L, bool default_scope = false) const
        {
            // Need to check if the class type of the signature is a base of this class
            object fn = make_function(L, f, default_scope, deduce_signature_t<F, Class>(), Policies());
            add_overload(object(from_stack(L, -1)), name, fn);
        }

        char const* name;
        F f;
    };
#ifdef _MSC_VER
# pragma pack(pop)
#endif
    template <class P, class T>
    using default_pointer = std::conditional_t<std::is_same_v<P, null_type>, luabind::unique_ptr<T>, P>;

    template <class Class, class Pointer, class Signature, class Policies>
    struct constructor_registration : registration
    {
        void register_(lua_State* L, bool default_scope = false) const
        {
            using pointer = default_pointer<Pointer, Class>;
            auto constructor = construct<Class, pointer, Signature>();
            object fn = make_function(L, constructor, default_scope, Signature(), Policies());
            add_overload(object(from_stack(L, -1)), "__init", fn);
        }
    };

    template <class T>
    using reference_result = std::conditional_t<
        std::is_pointer_v<T> || is_primitive_v<T>, T, std::add_lvalue_reference_t<T>>;

    template <class T>
    using reference_argument = std::conditional_t<
        std::is_pointer_v<T> || is_primitive_v<T>, T, std::add_lvalue_reference_t<std::add_const_t<T>>>;

    template <class T, class Policies>
    struct inject_dependency
    {
        using type = std::conditional_t<
            is_primitive_v<T> || meta::contains_v<Policies, postcall_injector<detail::no_dependency_policy>>,
            Policies, meta::push_back_t<Policies, policy::return_ref<1>>>;
    };

    template <class Class, class Get, class GetPolicies, class Set = null_type, class SetPolicies = no_policies>
    struct property_registration : registration
    {
        property_registration(char const* name, Get const& get, Set const& set = {}) :
            name(name),
            get(get),
            set(set)
        {}

        template <class F>
        static object make_get_free(lua_State* L, F const& f, bool default_scope)
        {
            return make_function(L, f, default_scope, GetPolicies());
        }

        template <class T, class D>
        static object make_get_member(lua_State* L, D T::* mem_ptr, bool default_scope)
        {
            using result_type = reference_result<D>;
            using get_signature = meta::type_list<result_type, Class const&>;
            using injected_list = typename inject_dependency<D, GetPolicies>::type;
            auto f = access_member_ptr<T, D, result_type>(mem_ptr);
            return make_function(L, f, default_scope, get_signature(), injected_list());
        }

        template <class F>
        static object make_set_free(lua_State* L, F const& f, bool default_scope)
        {
            return make_function(L, f, default_scope, deduce_signature_t<F>(), SetPolicies());
        }

        template <class T, class D>
        static object make_set_member(lua_State* L, D T::* mem_ptr, bool default_scope)
        {
            using argument_type = reference_argument<D>;
            using signature_type = meta::type_list<void, Class&, argument_type>;
            auto f = access_member_ptr<T, D>(mem_ptr);
            return make_function(L, f, default_scope, signature_type(), SetPolicies());
        }

        void register_(lua_State* L, bool default_scope = false) const
        {
            object context(from_stack(L, -1));
            object get_;
            if constexpr (std::is_member_object_pointer_v<Get>)
                get_ = make_get_member(L, get, default_scope);
            else
                get_ = make_get_free(L, get, default_scope);
            // if no setter was given
            if constexpr (is_null_type_v<Set>)
                context[name] = property(get_);
            else // if a setter was given
            {
                object set_;
                if constexpr (std::is_member_object_pointer_v<Set>)
                    set_ = make_set_member(L, set, default_scope);
                else
                    set_ = make_set_free(L, set, default_scope);
                context[name] = property(get_, set_);
            }
        }

        char const* name;
        Get get;
        Set set;
    };

    template <typename Default>
    constexpr bool is_func_v =
        std::is_function_v<std::remove_pointer_t<Default>> | std::is_member_function_pointer_v<Default>;
} // namespace luabind::detail

namespace luabind
{
    // registers a class in the lua environment
    template <class T, typename BaseOrBases, typename Holder, typename Wrapper>
    struct class_ : detail::class_base
    {
        using self_t = class_<T, BaseOrBases, Holder, Wrapper>;
        using BaseList = make_bases<BaseOrBases>;

    public:
        class_(const char* name = nullptr) :
            class_base(name),
            scope(*this)
        {
            init();
        }

        // virtual functions
        template <class F, typename... Injectors>
        class_& def(char const* name, F fn, policy_list<Injectors...> policies = {})
        {
            return virtual_def(name, fn, policies, null_type());
        }

        // IntelliSense bug squiggles the code, but it does compile!
        template <typename Ret, typename C, typename... Args, typename... Injectors>
        class_& def_nonconst(char const* name, Ret(C::* fn)(Args...), policy_list<Injectors...> policies = {})
        {
            return def(name, fn, policies);
        }

        // IntelliSense bug squiggles the code, but it does compile!
        template <typename Ret, typename C, typename... Args, typename... Injectors>
        class_& def_const(char const* name, Ret(C::* fn)(Args...) const, policy_list<Injectors...> policies = {})
        {
            return def(name, fn, policies);
        }

        template <class F, class Default, typename... Injectors>
        class_& def(char const* name, F fn, Default default_, policy_list<Injectors...> policies = {},
            std::enable_if_t<detail::is_func_v<Default>, Default> = nullptr)
        {
            return virtual_def(name, fn, policies, default_);
        }

        template <typename... Args, typename... Injectors>
        class_& def(constructor<Args...> sig, policy_list<Injectors...> policies = {})
        {
            return def_constructor(sig, policies);
        }

        template <class Getter, typename... Injectors>
        class_& property(const char* name, Getter g, policy_list<Injectors...> get_policies = {})
        {
            return property(name, g, null_type(), get_policies);
        }

        template <class Getter, class Setter, typename... GetPolicies, typename... SetPolicies>
        class_& property(const char* name, Getter g, Setter s,
            policy_list<GetPolicies...> = {}, policy_list<SetPolicies...> = {})
        {
            using get_policies = policy_list<GetPolicies...>;
            using set_policies = policy_list<SetPolicies...>;
            using prop_reg_type = detail::property_registration<T, Getter, get_policies, Setter, set_policies>;
            add_member(luabind_new<prop_reg_type>(name, g, s));
            return *this;
        }

        template <class C, class D, typename... Injectors>
        class_& def_readonly(const char* name, D C::* mem_ptr, policy_list<Injectors...> policies = {})
        {
            return property(name, mem_ptr, policies);
        }

        template <class C, class D, typename... GetInjectors, typename... SetInjectors>
        class_& def_readwrite(const char* name, D C::* mem_ptr,
            policy_list<GetInjectors...> get_injectors = {},
            policy_list<SetInjectors...> set_injectors = {})
        {
            return property(name, mem_ptr, mem_ptr, get_injectors, set_injectors);
        }

        template <class Derived, typename... Injectors>
        class_& def(detail::operator_<Derived>, policy_list<Injectors...> policies = {})
        {
            using policy_list_type = policy_list<Injectors...>;
            return def(Derived::name(), &Derived::template apply<T, policy_list_type>::execute, policies);
        }

        detail::enum_maker<self_t> enum_(const char*)
        {
            return detail::enum_maker<self_t>(*this);
        }

        template <class F, typename... Injectors>
        class_& index(F f, policy_list<Injectors...> policies = {})
        {
            return virtual_def("__index", f, policies, null_type());
        }

        // virtual functions
        template <class F, class Default, typename... Injectors>
        class_& index(F fn, Default default_, policy_list<Injectors...> policies = {},
            std::enable_if_t<detail::is_func_v<Default>, Default> = nullptr)
        {
            return virtual_def("__index", fn, policies, default_);
        }

        template <class F, class Default, typename... Injectors>
        class_& index(F fn, Default default_, policy_list<Injectors...> policies = {})
        {
            return virtual_def("__index", fn, policies, default_);
        }

        template <class F, typename... Injectors>
        class_& newindex(F f, policy_list<Injectors...> policies = {})
        {
            return virtual_def("__newindex", f, policies, null_type());
        }

        // virtual functions
        template <class F, class Default, typename... Injectors>
        class_& newindex(F fn, Default default_, policy_list<Injectors...> policies = {},
            std::enable_if_t<detail::is_func_v<Default>, Default> = nullptr)
        {
            return virtual_def("__newindex", fn, policies, default_);
        }

        template <class F, class Default, typename... Injectors>
        class_& newindex(F fn, Default default_, policy_list<Injectors...> policies = {})
        {
            return virtual_def("__newindex", fn, policies, default_);
        }

        detail::static_scope<self_t> scope;

    private:
        void init()
        {
            auto const& id = detail::registered_class<T>::id();
            auto const& w_id = detail::registered_class<Wrapper>::id();
            class_base::init(typeid(T), id, typeid(Wrapper), w_id);
            add_wrapper_cast<Wrapper>();
            generate_baseclass_list();
        }

        template <class S, typename OtherBaseOrBases, typename OtherWrapper>
        class_(const class_<S, OtherBaseOrBases, OtherWrapper>&);

        template <class Src, class Target>
        void add_downcast()
        {
            if constexpr (std::is_polymorphic_v<Src>)
            {
                auto const& src = detail::registered_class<Src>::id();
                auto const& target = detail::registered_class<Target>::id();
                add_cast(src, target, detail::dynamic_cast_<Src, Target>::execute);
            }
        }

        template <typename Class>
        void gen_base_info()
        {
            add_base(typeid(Class));
            auto const& src = detail::registered_class<T>::id();
            auto const& target = detail::registered_class<Class>::id();
            add_cast(src, target, detail::static_cast_<T, Class>::execute);
            add_downcast<Class, T>();
        }

        // this function generates conversion information
        // in the given class_rep structure. It will be able
        // to implicitly cast to the given template type
        template <typename... Classes>
        void gen_base_info(bases<Classes...>)
        {
            (gen_base_info<Classes>(), ...);
        }

        void generate_baseclass_list()
        {
            gen_base_info(BaseList());
        }

        void operator=(class_ const&);

        template <class U>
        void add_wrapper_cast()
        {
            if constexpr (!is_null_type_v<U>)
            {
                auto const& src = detail::registered_class<U>::id();
                auto const& target = detail::registered_class<T>::id();
                add_cast(src, target, detail::static_cast_<U, T>::execute);
                add_downcast<T, U>();
            }
        }

        // these handle default implementation of virtual functions
        template <class F, class Default, typename... Injectors>
        class_& virtual_def(char const* name, F const& fn, policy_list<Injectors...>, Default default_)
        {
            using policy_list_type = policy_list<Injectors...>;
            add_member(luabind_new<detail::memfun_registration<T, F, policy_list_type>>(name, fn));
            if constexpr (!is_null_type_v<Default>)
            {
                using memfun_reg_type = detail::memfun_registration<T, Default, policy_list_type>;
                add_default_member(luabind_new<memfun_reg_type>(name, default_));
            }
            return *this;
        }

        template <typename... SignatureElements, typename... Injectors>
        class_& def_constructor(constructor<SignatureElements...> const&, policy_list<Injectors...> const&)
        {
            using signature_type = meta::type_list<void, argument const&, SignatureElements...>;
            using policy_list_type = policy_list<Injectors...>;
            using construct_type = std::conditional_t<is_null_type_v<Wrapper>, T, Wrapper>;
            using registration_type = detail::constructor_registration<
                construct_type, Holder, signature_type, policy_list_type>;
            add_member(luabind_new<registration_type>());
            add_default_member(luabind_new<registration_type>());
            return *this;
        }
    };
} // namespace luabind

#ifdef _MSC_VER
# pragma warning(pop)
#endif
