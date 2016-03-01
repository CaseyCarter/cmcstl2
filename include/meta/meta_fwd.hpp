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

#include <utility>
#include <type_traits>

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
        template <typename... Ts>
        struct list;

        template <template <typename...> class>
        struct quote;

        template <typename T, template <T...> class F>
        struct quote_i;

        template <template <typename...> class C, typename... Ts>
        struct defer;

        template <typename T, template <T...> class C, T... Is>
        struct defer_i;

        template <typename T>
        constexpr bool is_list_v = false;

        template <typename... Ts>
        constexpr bool is_list_v<list<Ts...>> = true;

        template <typename...>
        concept bool True = true;

        template <typename T, typename U>
        concept bool Same = std::is_same<T, U>::value;

        template <template <typename...> class C, typename... Ts>
        concept bool Valid = requires
        {
            typename C<Ts...>;
        };

        template <typename T, template <T...> class C, T... Is>
        concept bool Valid_I = requires
        {
            typename C<Is...>;
        };

        template <typename T>
        concept bool Trait = requires
        {
            typename T::type;
        };

        template <typename T>
        concept bool Callable = requires
        {
            typename quote<T::template invoke>;
        };

        template <typename T>
        concept bool List = is_list_v<T>;

        // clang-format off
        template <typename T>
        concept bool Integral = requires
        {
            typename T::type;
            typename T::value_type;
            typename T::type::value_type;
            requires Same<typename T::value_type, typename T::type::value_type>;
            // \begin{BUGBUG} https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68434
            //{ T::value } -> Same<typename T::value_type>;
            //{ T::type::value } -> Same<typename T::value_type>;
            T::value;
            T::type::value;
            // requires Same<decltype(T::value), typename T::value_type>;
            // requires Same<decltype(T::type::value), typename T::value_type>;
            // \end{BUGBUG}
            { T {} } -> typename T::value_type;
            // Again: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68434
            //{ T{}() } -> Same<typename T::value_type>;
            T{}();
            requires Same<decltype(T{}()), typename T::value_type>;
        }
        && std::is_integral<typename T::value_type>::value
        && T::value == T::type::value
        && T{}() == T::value;
        // clang-format on

        template <typename T>
        struct id;

        template <Callable... Fs>
        struct compose;

        namespace extension
        {
            template <Callable F, typename L>
            struct apply;
        }

        using std::integer_sequence;

    } // inline namespace v1
} // namespace meta

#endif
