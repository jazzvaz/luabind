// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2013 The Luabind Authors

#pragma once

#include <tuple>

namespace luabind::meta
{
    struct type_list_tag {};
    struct index_list_tag {};

    // Index list and type list share pretty common patterns... is there a way to unify them?

    template <uint32_t>
    struct count {};

    template <uint32_t>
    struct index {};

    template <typename Type>
    struct type {};

    // Use this to unpack a parameter pack into a list of T's
    template <typename T, typename DontCare>
    struct unpack_helper
    {
        using type = T;
    };

    template <typename T>
    struct size;

    template <typename T>
    constexpr uint32_t size_v = size<T>::value;

    // Specializations so index lists can use the same syntax
    template <typename T, uint32_t Index>
    struct get;

    template <typename T, uint32_t index>
    using get_t = typename get<T, index>::type;

    template <typename T, uint32_t Index>
    constexpr uint32_t get_v = get<T, Index>::value;

    template <typename... Lists>
    struct join;

    template <typename List1, typename List2, typename... Lists>
    struct join<List1, List2, Lists...>
    {
        // Could try to join on both sides
        using type = typename join<typename join<List1, List2>::type, Lists...>::type;
    };

    template <typename T1, typename... Types>
    using join_t = typename join<T1, Types...>::type;

    template <typename T>
    using front_t = get_t<T, 0>;

    template <typename List, typename T>
    struct push_front;

    template <typename List, typename Type>
    using push_front_t = typename push_front<List, Type>::type;

    template <typename List, typename T>
    struct push_back;

    template <typename List, typename Type>
    using push_back_t = typename push_back<List, Type>::type;

    template <typename T>
    struct pop_front;

    template <typename T>
    using pop_front_t = typename pop_front<T>::type;

    template <typename T>
    struct pop_back;

    template <typename T>
    using pop_back_t = typename pop_back<T>::type;

    template <typename List, uint32_t Index, typename T>
    struct replace;

    template <typename List, uint32_t Index, typename T>
    using replace_t = typename replace<List, Index, T>::type;

    template <typename List, uint32_t Index, template <typename> class T>
    struct enwrap;

    template <typename List, uint32_t Index, template <typename> class T>
    using enwrap_t = typename enwrap<List, Index, T>::type;

    template <typename List, template <typename> class T>
    struct enwrap_all;

    template <typename List, template <typename> class T>
    using enwrap_all_t = typename enwrap_all<List, T>::type;

    template <typename List, uint32_t Index, template <typename> class T>
    struct transform;

    template <typename List, uint32_t Index, template <typename> class Function>
    using transform_t = typename transform<List, Index, Function>::type;

    template <typename List, template <typename> class Function>
    struct transform_all;

    template <typename List, template <typename> class Function>
    using transform_all_t = typename transform_all<List, Function>::type;

    template <typename T, uint32_t start, uint32_t end>
    struct make_sub_range;

    // Index list sub_range [start, end)
    template <typename T, uint32_t start, uint32_t end>
    using sub_range = typename make_sub_range<T, start, end>::type;

    // Used as terminator on type and index lists
    struct null_type {};

    template <typename... Types>
    struct type_list : public type_list_tag
    {
        template <uint32_t Index>
        using at = typename get<type_list, Index>::type;
    };

    template <typename... Types1, typename... Types2>
    type_list<Types1..., Types2...> operator|(const type_list<Types1...>&, const type_list<Types2...>&)
    {
        return type_list<Types1..., Types2...>();
    }

    template <typename T>
    struct is_typelist : public std::false_type
    {
        static const bool value = false; // XXX: already defined by false_type
    };

    template <typename... Types>
    struct is_typelist<type_list<Types...>> : public std::true_type
    {
        static const bool value = true; // XXX: already defined by true_type
    };

    // Find type
    template <typename TypeList, typename Type>
    struct contains;

    template <typename Type0, typename... Types, typename Type>
    struct contains<type_list<Type0, Types...>, Type> :
        std::disjunction<std::is_same<Type0, Type>, contains<type_list<Types...>, Type>>
    {};

    template <typename Type>
    struct contains<type_list<>, Type> : std::false_type
    {};

    template <typename TypeList, typename Type>
    constexpr bool contains_v = contains<TypeList, Type>::value;

    template <>
    struct size<type_list<>>
    {
        static constexpr int value = 0;
    };

    template <typename Type0, typename... Types>
    struct size<type_list<Type0, Types...>>
    {
        static constexpr int value = 1 + size<type_list<Types...>>::value;
    };

    template <typename... Types, typename Type>
    struct push_front<type_list<Types...>, Type>
    {
        using type = type_list<Type, Types...>;
    };

    template <typename... Types, typename Type>
    struct push_back<type_list<Types...>, Type>
    {
        using type = type_list<Types..., Type>;
    };

    template <typename Type0, typename... Types>
    struct pop_front<type_list<Type0, Types...>>
    {
        using type = type_list<Types...>;
    };

    template <>
    struct pop_front<type_list<>>
    {
        using type = type_list<>;
    };

    // Index access to type list
    template <typename Element0, typename... Elements, uint32_t Index>
    struct get<type_list<Element0, Elements...>, Index>
    {
        using type = typename get<type_list<Elements...>, Index - 1>::type;
    };

    template <typename Element0, typename... Elements>
    struct get<type_list<Element0, Elements...>, 0>
    {
        using type = Element0;
    };

    template <uint32_t Index>
    struct get<type_list<>, Index>
    {
        static_assert(size<type_list<int>>::value == 1, "Bad Index");
    };

    // Join Type Lists
    template <typename... Types1, typename... Types2>
    struct join<type_list<Types1...>, type_list<Types2...>>
    {
        using type = type_list<Types1..., Types2...>;
    };

    namespace detail
    {
        template <typename Head, typename Tail, typename Type, uint32_t Index>
        struct replace_helper;

        template <typename... Head, typename Current, typename... Tail, typename Type, uint32_t Index>
        struct replace_helper<type_list<Head...>, type_list<Current, Tail...>, Type, Index>
        {
            using type = typename replace_helper<type_list<Head..., Current>, type_list<Tail...>, Type, Index - 1>::type;
        };

        template <typename... Head, typename Current, typename... Tail, typename Type>
        struct replace_helper<type_list<Head...>, type_list<Current, Tail...>, Type, 0>
        {
            using type = type_list<Head..., Type, Tail...>;
        };
    } // namespace detail

    template <typename... Types, uint32_t Index, typename Type>
    struct replace<type_list<Types...>, Index, Type>
    {
        using TypeList = type_list<Types...>;

        using type = join_t<
            sub_range<TypeList, 0, Index>,
            type_list<Type>,
            sub_range<TypeList, Index + 1, sizeof...(Types)>
        >;
    };

    // Enwrap all elements of a type list in an template
    template <typename... Types, uint32_t Index, template <typename> class Enwrapper>
    struct enwrap<type_list<Types...>, Index, Enwrapper>
    {
        using type = join_t<
            sub_range<type_list<Types...>, 0, Index>,
            Enwrapper<get_t<type_list<Types...>, Index>>,
            sub_range<type_list<Types...>, Index + 1, sizeof...(Types)>
        >;
    };

    template <typename... Types, template <typename> class Enwrapper>
    struct enwrap_all<type_list<Types...>, Enwrapper>
    {
        using type = type_list<Enwrapper<Types>...>;
    };

    // Transform a certain element of a type list
    template <typename T, uint32_t Index, template <typename> class Function>
    struct transform;

    template <typename... Types, uint32_t Index, template <typename> class Function>
    struct transform<type_list<Types...>, Index, Function>
    {
        using type = join_t<
            sub_range<type_list<Types...>, 0, Index>,
            typename Function<get_t<type_list<Types...>, Index>>::type,
            sub_range<type_list<Types...>, Index + 1, sizeof...(Types)>
        >;
    };

    // Transform all elements of a type list
    template <typename... Types, template <typename>  class Function>
    struct transform_all<type_list<Types...>, Function>
    {
        using type = type_list<typename Function<Types>::type...>;
    };

    // Tuple from type list
    template <class TypeList>
    struct make_tuple;

    template <typename... Types>
    struct make_tuple<type_list<Types...>>
    {
        using type = std::tuple<Types...>;
    };

    template <class TypeList>
    using make_tuple_t = typename make_tuple<TypeList>::type;

    // Type selection
    template <typename ConvertibleToTrueFalse, typename Result>
    struct case_ : public ConvertibleToTrueFalse
    {
        using type = Result;
    };

    template <typename Result>
    struct default_
    {
        using type = Result;
    };

    template <typename Case, typename... CaseList>
    struct select_
    {
        using type = std::conditional_t<
            std::is_convertible_v<Case, std::true_type>,
            typename Case::type, typename select_<CaseList...>::type>;
    };

    template <typename Case>
    struct select_<Case>
    {
        using type = std::conditional_t<
            std::is_convertible_v<Case, std::true_type>,
            typename Case::type, null_type>;
    };

    template <typename T>
    struct select_<default_<T>>
    {
        using type = typename default_<T>::type;
    };

    template <typename Case, typename... CaseList>
    using select_t = typename select_<Case, CaseList...>::type;

    // Create index lists to expand on type_lists
    template <uint32_t... Indices>
    struct index_list : public index_list_tag
    {};

    // Index index list
    namespace detail
    {
        template <uint32_t Index, uint32_t Value0, uint32_t... Values>
        struct get_iterate
        {
            static constexpr uint32_t value = get_iterate<Index - 1, Values...>::value;
        };

        template <uint32_t Value0, uint32_t... Values>
        struct get_iterate<0, Value0, Values...>
        {
            static constexpr uint32_t value = Value0;
        };
    } // namespace detail

    template <uint32_t... Values, uint32_t Index>
    struct get<index_list<Values...>, Index>
    {
        static_assert(sizeof...(Values) > Index, "Bad Index");
        static constexpr uint32_t value = detail::get_iterate<Index, Values...>::value;
    };

    // Index list size
    template <uint32_t... Values>
    struct size<index_list<Values...>>
    {
        static constexpr uint32_t value = sizeof...(Values);
    };

    // Index list push front
    template <uint32_t... Indices, uint32_t Index>
    struct push_front<index_list<Indices...>, index<Index>>
    {
        using type = index_list<Index, Indices...>;
    };

    // Index list push back
    template <uint32_t... Indices, uint32_t Index>
    struct push_back<index_list<Indices...>, index<Index>>
    {
        using type = index_list<Indices..., Index>;
    };

    // Index list pop_front
    template <uint32_t Index0, uint32_t... Indices>
    struct pop_front<index_list<Index0, Indices...>>
    {
        using type = index_list<Indices...>;
    };

    template <>
    struct pop_front<index_list<>>
    {
        using type = index_list<>;
    };

    // Index list range creation
    namespace detail
    {
        template <uint32_t curr, uint32_t end, uint32_t... Indices>
        struct make_index_range : make_index_range<curr + 1, end, Indices..., curr>
        {};

        template <uint32_t end, uint32_t... Indices>
        struct make_index_range<end, end, Indices...>
        {
            using type = index_list<Indices...>;
        };
    } // namespace detail

    // make_index_range<start, end>
    // Creates the index list list of range [start, end)
    template <uint32_t start, uint32_t end>
    struct make_index_range
    {
        static_assert(end >= start, "end must be greater than or equal to start");
        using type = typename detail::make_index_range<start, end>::type;
    };

    template <uint32_t start, uint32_t end>
    using index_range = typename make_index_range<start, end>::type;

    namespace detail
    {
        // These implementation are not really efficient...
        template <typename SourceList, typename IndexList>
        struct sub_range_index;

        template <typename SourceList, uint32_t... Indices>
        struct sub_range_index<SourceList, index_list<Indices...>>
        {
            using type = index_list<get<SourceList, Indices>::value...>;
        };

        template <typename SourceList, typename IndexList>
        struct sub_range_type;

        template <typename SourceList, uint32_t... Indices>
        struct sub_range_type<SourceList, index_list<Indices...>>
        {
            using type = type_list<typename get<SourceList, Indices>::type...>;
        };
    } // namespace detail

    template <uint32_t start, uint32_t end, uint32_t... Indices>
    struct make_sub_range<index_list<Indices...>, start, end>
    {
        static_assert(end >= start, "end must be greater or equal to start");
        using type = typename detail::sub_range_index<index_list<Indices...>, index_range<start, end>>::type;
    };

    template <uint32_t start, uint32_t end, typename... Types>
    struct make_sub_range<type_list<Types...>, start, end>
    {
        static_assert(end >= start, "end must be greater or equal to start");
        using type = typename detail::sub_range_type<type_list<Types...>, index_range<start, end>>::type;
    };

    // Index list sum
    namespace detail
    {
        template <typename T, T... Values>
        struct sum_values;

        template <typename T, T Value0, T... Values>
        struct sum_values<T, Value0, Values...>
        {
            static constexpr T value = Value0 + sum_values<T, Values...>::value;
        };

        template <typename T>
        struct sum_values<T>
        {
            static constexpr T value = 0;
        };
    } // namespace detail

    template <typename T>
    struct sum;

    template <uint32_t... Args>
    struct sum<index_list<Args...>>
    {
        static constexpr uint32_t value = detail::sum_values<uint32_t, Args...>::value;
    };

    template <typename T>
    constexpr uint32_t sum_v = sum<T>::value;

    template <typename... ConvertiblesToTrueFalse>
    struct and_;

    template <typename Convertible0, typename... Convertibles>
    struct and_<Convertible0, Convertibles...> :
        std::conditional_t<
        std::is_convertible_v<Convertible0, std::true_type>,
        and_<Convertibles...>, std::false_type>
    {};

    template <>
    struct and_<> : std::true_type
    {};

    template <typename... ConvertiblesToTrueFalse>
    struct or_;

    template <typename Convertible0, typename... Convertibles>
    struct or_<Convertible0, Convertibles...> :
        std::conditional_t<
        std::is_convertible_v<Convertible0, std::true_type>,
        std::true_type, or_<Convertibles...>>
    {};

    template <>
    struct or_<> : std::true_type
    {};
} // namespace luabind::meta
