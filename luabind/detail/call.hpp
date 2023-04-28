// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <typeinfo>
#include <luabind/detail/meta.hpp>
#include <luabind/detail/policy.hpp>
#include <luabind/yield_policy.hpp>
#include <luabind/detail/decorate_type.hpp>
#include <luabind/detail/object.hpp>
#include <tuple>

#pragma warning(push)
#pragma warning(disable : 4251)

namespace luabind::detail
{
    struct invoke_context;

    struct LUABIND_API function_object
    {
        function_object(lua_CFunction entry) :
            entry(entry),
            next(nullptr)
        {}

        virtual ~function_object() = default;

        virtual int call(lua_State* L, invoke_context& ctx, int args) /* const */ = 0;
        virtual int format_signature(lua_State* L, char const* function, bool concat = true) const = 0;

        lua_CFunction entry;
        luabind::string name;
        function_object* next;
        object keepalive;
    };

    struct LUABIND_API invoke_context
    {
        operator bool() const { return candidate_index == 1; }

        void format_error(lua_State* L, function_object const* overloads) const;

        int best_score = std::numeric_limits<int>::max();
        static constexpr size_t max_candidates = 10;
        function_object const* candidates[max_candidates] = {};
        size_t extra_candidates = 0;
        size_t candidate_index = 0;
    };

    // Compute Stack Indices
    // Given the list of argument converter arities, computes the stack indices that each converter addresses.
    template <typename ConsumedList, uint32_t Sum, uint32_t... StackIndices>
    struct compute_stack_indices;

    template <uint32_t Head, uint32_t... Tail, uint32_t Sum, uint32_t... StackIndices>
    struct compute_stack_indices<meta::index_list<Head, Tail...>, Sum, StackIndices...>
    {
        using next = compute_stack_indices<meta::index_list<Tail...>, Sum + Head, StackIndices..., Sum>;
        using type = typename next::type;
    };

    template <uint32_t Sum, uint32_t... StackIndices>
    struct compute_stack_indices<meta::index_list<>, Sum, StackIndices...>
    {
        using type = meta::index_list<StackIndices...>;
    };

    template <typename ConsumedList, uint32_t Sum, uint32_t... StackIndices>
    using compute_stack_indices_t = typename compute_stack_indices<ConsumedList, Sum, StackIndices...>::type;

    template <typename StackIndexList, typename Policy>
    void postcall(lua_State* L, int results, StackIndexList, postcall_injector<Policy>)
    {
        Policy::postcall(L, results, StackIndexList());
    }

    template <typename StackIndexList, typename Policy, uint32_t Index>
    void postcall(lua_State* L, int results, StackIndexList, converter_injector<Index, Policy>)
    {
        if constexpr (converter_injector<Index, Policy>::has_postcall)
            Policy::postcall(L, results, StackIndexList());
    }

    template <typename... PolicyInjectors, typename StackIndexList>
    void postcall(lua_State* L, int results, StackIndexList, meta::type_list<PolicyInjectors...>)
    {
        (postcall(L, results, StackIndexList(), PolicyInjectors()), ...);
    }

    template <typename ArgList, typename PolicyList, typename ArgIndexList>
    struct argument_converter_list;

    template <typename ArgList, typename PolicyList, uint32_t... Indices>
    struct argument_converter_list<ArgList, PolicyList, meta::index_list<Indices...>>
    {
        using type = meta::type_list< // policy 0 is for return value, so add 1
            specialized_converter_policy_n<Indices + 1, PolicyList, meta::get_t<ArgList, Indices>, lua_to_cpp>...>;
    };

    template <typename ArgList, typename PolicyList, typename ArgIndexList>
    using argument_converter_list_t = typename argument_converter_list<ArgList, PolicyList, ArgIndexList>::type;

    template <typename ConvList>
    struct build_consumed_list;

    template <typename... Converters>
    struct build_consumed_list<meta::type_list<Converters...>>
    {
        using type = meta::index_list<Converters::consumed_args...>;
    };

    template <typename RArgList, typename PolicyList>
    struct invoke_traits;

    // Specialization for free functions
    template <typename Result, typename... Args, typename PolicyList>
    struct invoke_traits<meta::type_list<Result, Args...>, PolicyList>
    {
        using signature_list = meta::type_list<Result, Args...>;
        using policy_list = PolicyList;
        using result_type = Result;
        using result_converter = specialized_converter_policy_n<0, PolicyList, result_type, cpp_to_lua>;
        using arg_list = meta::type_list<Args...>;
        using decorated_arg_list = meta::type_list<decorate_type_t<Args>...>;
        // note that this is 0-based, so whenever you want to fetch from the converter list, you need to add 1
        using arg_index_list = meta::index_range<0, sizeof...(Args)>; // start:0, end:sizeof(args)
        using arg_converter_list = argument_converter_list_t<arg_list, policy_list, arg_index_list>;
        using arg_converter_tuple_type = meta::make_tuple_t<arg_converter_list>;
        using consumed_list = typename build_consumed_list<arg_converter_list>::type;
        using stack_index_list = compute_stack_indices_t<consumed_list, 1>;
        static constexpr int arity = meta::sum_v<consumed_list>;
    };

    template <typename PolicyList, typename RArgList, typename F>
    struct invoke_struct
    {
        using traits = invoke_traits<RArgList, PolicyList>;
        using converter_tuple = typename traits::arg_converter_tuple_type;
        using result_converter = typename traits::result_converter;
        using decorated_list = typename traits::decorated_arg_list;
        using stack_indices = typename traits::stack_index_list;
        using arg_index_list = typename traits::arg_index_list;
        using signature_list = typename traits::signature_list;
        using result_type = typename traits::result_type;

        template <uint32_t Index>
        static decltype(auto) convert_to_cpp(lua_State* L, converter_tuple& cvt)
        {
            auto& converter = std::get<Index>(cvt);
            using decorated_type = meta::get_t<decorated_list, Index>;
            auto i = meta::get_v<stack_indices, Index>;
            return converter.to_cpp(L, decorated_type(), i);
        }

        template <uint32_t Index>
        static decltype(auto) convert_postcall(lua_State* L, converter_tuple& cvt)
        {
            auto& converter = std::get<Index>(cvt);
            using decorated_type = meta::get_t<decorated_list, Index>;
            auto i = meta::get_v<stack_indices, Index>;
            return converter.converter_postcall(L, decorated_type(), i);
        }

        template <uint32_t... Indices>
        static void call_free(lua_State* L, F& f, meta::index_list<Indices...>, converter_tuple& cvt)
        {
            if constexpr (!std::is_void_v<result_type>)
                result_converter().to_lua(L, f(convert_to_cpp<Indices>(L, cvt)...));
            else
                f(convert_to_cpp<Indices>(L, cvt)...);
            (convert_postcall<Indices>(L, cvt), ...);
        }

        template <uint32_t IThis, uint32_t... Indices>
        static void call_member(lua_State* L, F& f, meta::index_list<IThis, Indices...>, converter_tuple& cvt)
        {
            auto& obj = convert_to_cpp<IThis>(L, cvt);
            if constexpr (!std::is_void_v<result_type>)
                result_converter().to_lua(L, (obj.*f)(convert_to_cpp<Indices>(L, cvt)...));
            else
                (obj.*f)(convert_to_cpp<Indices>(L, cvt)...);
            (convert_postcall<Indices>(L, cvt), ...);
        }

        template <uint32_t Index>
        static int match_arg(lua_State* L, converter_tuple& cvt)
        {
            auto& converter = std::get<Index>(cvt);
            using decorated_type = meta::get_t<decorated_list, Index>;
            constexpr auto stackIndex = meta::get_v<stack_indices, Index>;
            return converter.match(L, decorated_type(), stackIndex);
        }

        template <uint32_t... Indices>
        static int match_args(lua_State* L, meta::index_list<Indices...>, converter_tuple& cvt)
        {
            return (match_arg<Indices>(L, cvt) + ... + 0);
        }

        static int invoke(lua_State* L, function_object const& self, invoke_context& ctx, F& f)
        {
            int const argc = lua_gettop(L);
#ifdef LUABIND_ALLOW_MISSING_ARGUMENTS
            if (!self.next)
            {
                converter_tuple cvt;
                ctx.best_score = match_args(L, arg_index_list(), cvt);
                ctx.candidates[0] = &self;
                ctx.candidate_index = 1;
                ctx.extra_candidates = 0;
                return invoke(L, ctx, f, argc, cvt);
            }
#endif // LUABIND_ALLOW_MISSING_ARGUMENTS
            return invoke_best_match(L, self, ctx, f, argc);
        }

        template <typename ConvTuple>
        static int invoke(lua_State* L, invoke_context& ctx, F& f, int args, ConvTuple& conv_tuple)
        {
            int results = 0;
            if constexpr (std::is_member_function_pointer_v<F>)
                call_member(L, f, arg_index_list(), conv_tuple);
            else
                call_free(L, f, arg_index_list(), conv_tuple);
            results = lua_gettop(L) - args;
            if constexpr (has_call_policy_v<PolicyList, yield_policy>)
                return -results - 1;
            using indices_w_arity = meta::push_front_t<stack_indices, meta::index<traits::arity>>;
            postcall(L, results, indices_w_arity(), PolicyList());
            return results;
        }

        static int invoke_best_match(lua_State* L, function_object const& self, invoke_context& ctx, F& f, int args)
        {
            // Things to remember:
            // 0 is the perfect match. match > 0 means that objects had to be cast,
            // where the value is the total distance of all arguments to their given
            // types (graph distance).
            // This is why we can say MaxArguments = 100, MaxDerivationDepth = 100,
            // so no match will be > 100*100=10k and -10k1 absorbs every match.
            // This gets rid of the awkward checks during converter match traversal.
            int score = no_match;
            // Even match needs the tuple, since pointer_converters buffer the cast result
            converter_tuple cvt;
            if (traits::arity == args) {
                score = match_args(L, arg_index_list(), cvt);
            }
            if (score >= 0 && score < ctx.best_score)
            {
                ctx.best_score = score;
                ctx.candidates[0] = &self;
                ctx.candidate_index = 1;
                ctx.extra_candidates = 0;
            }
            else if (score == ctx.best_score)
            {
                if (ctx.candidate_index < invoke_context::max_candidates)
                    ctx.candidates[ctx.candidate_index++] = &self;
                else
                    ctx.extra_candidates++;
            }
            int results = 0;
            if (self.next)
                results = self.next->call(L, ctx, args);
            if (score == ctx.best_score && ctx.candidate_index == 1)
                results = invoke(L, ctx, f, args, cvt);

            return results;
        }
    };

    template <typename PolicyList, typename RArgList, typename F>
    int invoke_best_match(lua_State* L, function_object const& self, invoke_context& ctx, F& f, int args)
    {
        return invoke_struct<PolicyList, RArgList, F>::invoke_best_match(L, self, ctx, f, args);
    }

    template <typename PolicyList, typename RArgList, typename F>
    int invoke(lua_State* L, function_object const& self, invoke_context& ctx, F& f)
    {
        return invoke_struct<PolicyList, RArgList, F>::invoke(L, self, ctx, f);
    }
} // namespace luabind::detail

#pragma warning(pop) 