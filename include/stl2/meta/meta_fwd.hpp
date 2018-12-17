/// \file meta_fwd.hpp Forward declarations
//
// Meta library
//
//  Copyright Eric Niebler 2014-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef META_FWD_HPP
#define META_FWD_HPP

#include <type_traits>
#include <utility>

#if !defined(__cpp_concepts) || __cpp_concepts == 0
#error Nothing here will work without support for C++ concepts.
#elif __cpp_concepts <= 201507L
#define META_CONCEPT concept bool
// TS concepts subsumption barrier for atomic expressions
#define META_CONCEPT_BARRIER(...) ::meta::detail::bool_<__VA_ARGS__>
#else
#define META_CONCEPT concept
#define META_CONCEPT_BARRIER(...) __VA_ARGS__
#endif

#ifndef META_DISABLE_DEPRECATED_WARNINGS
#ifdef __cpp_attribute_deprecated
#define META_DEPRECATED(MSG) [[deprecated(MSG)]]
#else
#if defined(__clang__) || defined(__GNUC__)
#define META_DEPRECATED(MSG) __attribute__((deprecated(MSG)))
#elif defined(_MSC_VER)
#define META_DEPRECATED(MSG) __declspec(deprecated(MSG))
#else
#define META_DEPRECATED(MSG)
#endif
#endif
#else
#define META_DEPRECATED(MSG)
#endif

namespace meta
{
    inline namespace v1
    {
        namespace detail
        {
            template<bool B>
            inline constexpr bool bool_ = B;

            template<auto> struct require_constant; // not defined
        }

        template<typename... Ts>
        struct list;

        template<template<typename...> class>
        struct quote;

        template<typename T, template<T...> class F>
        struct quote_i;

        template<template<typename...> class C, typename... Ts>
        struct defer;

        template<typename T, template<T...> class C, T... Is>
        struct defer_i;

        template<typename...>
        META_CONCEPT True = META_CONCEPT_BARRIER(true);

        template<typename T, typename U>
        META_CONCEPT Same =
#if defined(__clang__)
            META_CONCEPT_BARRIER(__is_same(T, U));
#elif defined(__GNUC__)
            META_CONCEPT_BARRIER(__is_same_as(T, U));
#else
            META_CONCEPT_BARRIER(std::is_same_v<T, U>);
#endif

        template<template<typename...> class C, typename... Ts>
        META_CONCEPT Valid = requires
        {
            typename C<Ts...>;
        };

        template<typename T, template<T...> class C, T... Is>
        META_CONCEPT Valid_I = requires
        {
            typename C<Is...>;
        };

        template<typename T>
        META_CONCEPT Trait = requires
        {
            typename T::type;
        };

        template<typename T>
        META_CONCEPT Invocable = requires
        {
            typename quote<T::template invoke>;
        };

        /// is_v
        /// Test whether a type \p T is an instantiation of class
        /// template \p C.
        /// \ingroup trait
        template<typename, template<typename...> class>
        inline constexpr bool is_v = false;
        template<typename... Ts, template<typename...> class C>
        inline constexpr bool is_v<C<Ts...>, C> = true;

        template<typename T>
        META_CONCEPT List = is_v<T, list>;

        // clang-format off
        template<typename T>
        META_CONCEPT Integral = requires
        {
            typename T::type;
            typename T::value_type;
            typename T::type::value_type;
        }
        && Same<typename T::value_type, typename T::type::value_type>
        && std::is_integral_v<typename T::value_type>
        && requires
        {
            // { T::value } -> Same<const typename T::value_type&>;
            T::value;
            requires Same<decltype(T::value), const typename T::value_type>;
            typename detail::require_constant<T::value>;

            // { T::type::value } -> Same<const typename T::value_type&>;
            T::type::value;
            requires Same<decltype(T::type::value), const typename T::value_type>;
            typename detail::require_constant<T::type::value>;
            requires T::value == T::type::value;

            // { T{}() } -> Same<typename T::value_type>;
            T{}();
            requires Same<decltype(T{}()), typename T::value_type>;
            typename detail::require_constant<T{}()>;
            requires T{}() == T::value;

            { T{} } -> typename T::value_type;
        };
        // clang-format on

        template<typename T>
        struct id;

        template<Invocable... Fs>
        struct compose_;

        namespace extension
        {
            template<Invocable F, typename L>
            struct apply;
        }

        using std::integer_sequence;
    } // inline namespace v1
} // namespace meta

#endif
