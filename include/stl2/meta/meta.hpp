/// \file meta.hpp Tiny meta-programming library.
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

#ifndef META_HPP
#define META_HPP

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <stl2/meta/meta_fwd.hpp>

#if defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdocumentation-deprecated-sync"
#endif

/// \defgroup meta Meta
///
/// A tiny metaprogramming library

/// \defgroup trait Trait
/// Trait invocation/composition.
/// \ingroup meta

/// \defgroup invocation Invocation
/// Trait invocation
/// \ingroup trait

/// \defgroup composition Composition
/// Trait composition
/// \ingroup trait

/// \defgroup logical Logical
/// Logical operations
/// \ingroup meta

/// \defgroup algorithm Algorithms
/// Algorithms.
/// \ingroup meta

/// \defgroup query Query/Search
/// Query and search algorithms
/// \ingroup algorithm

/// \defgroup transformation Transformation
/// Transformation algorithms
/// \ingroup algorithm

/// \defgroup runtime Runtime
/// Runtime algorithms
/// \ingroup algorithm

/// \defgroup datatype Datatype
/// Datatypes.
/// \ingroup meta

/// \defgroup list List
/// \ingroup datatype

/// \defgroup integral Integer sequence
/// Equivalent to C++14's `std::integer_sequence`
/// \ingroup datatype

/// \defgroup extension Extension
/// Extend meta with your own datatypes.
/// \ingroup datatype

/// \defgroup math Math
/// Integral constant arithmetic.
/// \ingroup meta

/// \defgroup lazy_trait lazy
/// \ingroup trait

/// \defgroup lazy_invocation lazy
/// \ingroup invocation

/// \defgroup lazy_composition lazy
/// \ingroup composition

/// \defgroup lazy_logical lazy
/// \ingroup logical

/// \defgroup lazy_query lazy
/// \ingroup query

/// \defgroup lazy_transformation lazy
/// \ingroup transformation

/// \defgroup lazy_list lazy
/// \ingroup list

/// \defgroup lazy_datatype lazy
/// \ingroup datatype

/// \defgroup lazy_math lazy
/// \ingroup math

/// Tiny metaprogramming library
namespace meta
{
    /// \cond
    inline namespace v1
    {
        /// \endcond

        namespace detail
        {
            /// Returns a \p T nullptr
            template <typename T>
            constexpr T *nullptr_v = nullptr;
        } // namespace detail

        /// An empty type.
        /// \ingroup datatype
        struct nil_
        {
        };

        /// Type alias for \p T::type.
        /// \ingroup invocation
        template <Trait T>
        using _t = typename T::type;

        /// Variable alias for \c T::type::value
        /// \note Requires C++14 or greater.
        /// \ingroup invocation
        template <Integral T>
        constexpr typename T::type::value_type _v = T::type::value;

        /// Lazy versions of meta actions
        namespace lazy
        {
            /// \sa `meta::_t`
            /// \ingroup lazy_invocation
            template <typename T>
            using _t = defer<_t, T>;
        }

        /// An integral constant wrapper for \c std::size_t.
        /// \ingroup integral
        template <std::size_t N>
        using size_t = std::integral_constant<std::size_t, N>;

        /// An integral constant wrapper for \c bool.
        /// \ingroup integral
        template <bool B>
        using bool_ = std::integral_constant<bool, B>;

        /// An integral constant wrapper for \c int.
        /// \ingroup integral
        template <int I>
        using int_ = std::integral_constant<int, I>;

        /// An integral constant wrapper for \c char.
        /// \ingroup integral
        template <char Ch>
        using char_ = std::integral_constant<char, Ch>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Math operations
        /// An integral constant wrapper around the result of incrementing the wrapped integer \c
        /// T::type::value.
        template <Integral T>
        using inc = std::integral_constant<decltype(_v<T> + 1), _v<T> + 1>;

        /// An integral constant wrapper around the result of decrementing the wrapped integer \c
        /// T::type::value.
        template <Integral T>
        using dec = std::integral_constant<decltype(_v<T> - 1), _v<T> - 1>;

        /// An integral constant wrapper around the result of adding the two wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using plus = std::integral_constant<decltype(_v<T> + _v<U>), _v<T> + _v<U>>;

        /// An integral constant wrapper around the result of subtracting the two wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using minus = std::integral_constant<decltype(_v<T> - _v<U>), _v<T> - _v<U>>;

        /// An integral constant wrapper around the result of multiplying the two wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using multiplies = std::integral_constant<decltype(_v<T> * _v<U>), _v<T> * _v<U>>;

        /// An integral constant wrapper around the result of dividing the two wrapped integers \c
        /// T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using divides = std::integral_constant<decltype(_v<T> / _v<U>), _v<T> / _v<U>>;

        /// An integral constant wrapper around the result of negating the wrapped integer
        /// \c T::type::value.
        /// \ingroup math
        template <Integral T>
        using negate = std::integral_constant<decltype(-_v<T>), -_v<T>>;

        /// An integral constant wrapper around the remainder of dividing the two wrapped integers
        /// \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using modulus = std::integral_constant<decltype(_v<T> % _v<U>), _v<T> % _v<U>>;

        /// A Boolean integral constant wrapper around the result of comparing \c T::type::value and
        /// \c U::type::value for equality.
        /// \ingroup math
        template <Integral T, Integral U>
        using equal_to = bool_<_v<T> == _v<U>>;

        /// A Boolean integral constant wrapper around the result of comparing \c T::type::value and
        /// \c U::type::value for inequality.
        /// \ingroup math
        template <Integral T, Integral U>
        using not_equal_to = bool_<_v<T> != _v<U>>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is greater than
        /// \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <Integral T, Integral U>
        using greater = bool_<(_v<T>> _v<U>)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is less than \c
        /// U::type::value; \c false, otherwise.
        /// \ingroup math
        template <Integral T, Integral U>
        using less = bool_<(_v<T> < _v<U>)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is greater than
        /// or equal to \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <Integral T, Integral U>
        using greater_equal = bool_<(_v<T> >= _v<U>)>;

        /// A Boolean integral constant wrapper around \c true if \c T::type::value is less than or
        /// equal to \c U::type::value; \c false, otherwise.
        /// \ingroup math
        template <Integral T, Integral U>
        using less_equal = bool_<(_v<T> <= _v<U>)>;

        /// An integral constant wrapper around the result of bitwise-and'ing the two wrapped
        /// integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using bit_and = std::integral_constant<decltype(_v<T> & _v<U>), _v<T> & _v<U>>;

        /// An integral constant wrapper around the result of bitwise-or'ing the two wrapped
        /// integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using bit_or = std::integral_constant<decltype(_v<T> | _v<U>), _v<T> | _v<U>>;

        /// An integral constant wrapper around the result of bitwise-exclusive-or'ing the two
        /// wrapped integers \c T::type::value and \c U::type::value.
        /// \ingroup math
        template <Integral T, Integral U>
        using bit_xor = std::integral_constant<decltype(_v<T> ^ _v<U>), _v<T> ^ _v<U>>;

        /// An integral constant wrapper around the result of bitwise-complementing the wrapped
        /// integer \c T::type::value.
        /// \ingroup math
        template <Integral T>
        using bit_not = std::integral_constant<decltype(~_v<T>), ~_v<T>>;

        namespace lazy
        {
            /// \sa 'meta::int'
            /// \ingroup lazy_math
            template <typename T>
            using inc = defer<inc, T>;

            /// \sa 'meta::dec'
            /// \ingroup lazy_math
            template <typename T>
            using dec = defer<dec, T>;

            /// \sa 'meta::plus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using plus = defer<plus, T, U>;

            /// \sa 'meta::minus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using minus = defer<minus, T, U>;

            /// \sa 'meta::multiplies'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using multiplies = defer<multiplies, T, U>;

            /// \sa 'meta::divides'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using divides = defer<divides, T, U>;

            /// \sa 'meta::negate'
            /// \ingroup lazy_math
            template <typename T>
            using negate = defer<negate, T>;

            /// \sa 'meta::modulus'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using modulus = defer<modulus, T, U>;

            /// \sa 'meta::equal_to'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using equal_to = defer<equal_to, T, U>;

            /// \sa 'meta::not_equal_t'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using not_equal_to = defer<not_equal_to, T, U>;

            /// \sa 'meta::greater'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using greater = defer<greater, T, U>;

            /// \sa 'meta::less'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using less = defer<less, T, U>;

            /// \sa 'meta::greater_equal'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using greater_equal = defer<greater_equal, T, U>;

            /// \sa 'meta::less_equal'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using less_equal = defer<less_equal, T, U>;

            /// \sa 'meta::bit_and'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_and = defer<bit_and, T, U>;

            /// \sa 'meta::bit_or'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_or = defer<bit_or, T, U>;

            /// \sa 'meta::bit_xor'
            /// \ingroup lazy_math
            template <typename T, typename U>
            using bit_xor = defer<bit_xor, T, U>;

            /// \sa 'meta::bit_not'
            /// \ingroup lazy_math
            template <typename T>
            using bit_not = defer<bit_not, T>;
        }

        /// Evaluate the Invocable \p Fn with the arguments \p Args.
        /// \ingroup invocation
        template <Invocable Fn, typename... Args>
        using invoke = typename Fn::template invoke<Args...>;

        /// Lazy versions of meta actions
        namespace lazy
        {
            /// \sa `meta::invoke`
            /// \ingroup lazy_invocation
            template <typename Fn, typename... Args>
            using invoke = defer<invoke, Fn, Args...>;
        }

        /// An alias for `void`.
        /// \ingroup trait
        template <typename...>
        using void_ = void;

        /// \cond
        namespace detail
        {
            template <template <typename...> class, typename...>
            struct defer_
            {
            };

            template <template <typename...> class C, typename... Ts>
            requires Valid<C, Ts...> struct defer_<C, Ts...>
            {
                using type = C<Ts...>;
            };

            template <typename T, template <T...> class, T...>
            struct defer_i_
            {
            };

            template <typename T, template <T...> class C, T... Is>
            requires Valid_I<T, C, Is...> struct defer_i_<T, C, Is...>
            {
                using type = C<Is...>;
            };

            template <typename T>
            using _t_t = _t<_t<T>>;
        } // namespace detail
        /// \endcond

        /// `true` if `T::type` exists and names a type; `false` otherwise.
        /// \ingroup trait
        template <typename>
        constexpr bool is_trait_v = false;

        Trait{T}
        constexpr bool is_trait_v<T> = true;

        /// An alias for `std::true_type` if `T::type` exists and names a type; otherwise, it's an
        /// alias for `std::false_type`.
        /// \ingroup trait
        template <typename T>
        using is_trait = bool_<is_trait_v<T>>;

        /// `true` if `T::invoke` exists and names a class template; `false` otherwise.
        /// \ingroup trait
        template <typename>
        constexpr bool is_callable_v = false;

        Invocable{T}
        constexpr bool is_callable_v<T> = true;

        /// An alias for `std::true_type` if `T::invoke` exists and names a class template;
        /// otherwise, it's an alias for `std::false_type`.
        /// \ingroup trait
        template <typename T>
        using is_callable = bool_<is_callable_v<T>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer
        /// A wrapper that defers the instantiation of a template \p C with type parameters \p Ts in
        /// a \c lambda or \c let expression.
        ///
        /// In the code below, the lambda would ideally be written as
        /// `lambda<_a,_b,push_back<_a,_b>>`, however this fails since `push_back` expects its first
        /// argument to be a list, not a placeholder. Instead, we express it using \c defer as
        /// follows:
        ///
        /// \code
        /// template <typename L>
        /// using reverse = reverse_fold<L, list<>, lambda<_a, _b, defer<push_back, _a, _b>>>;
        /// \endcode
        ///
        /// \ingroup invocation
        template <template <typename...> class C, typename... Ts>
        struct defer : detail::defer_<C, Ts...>
        {
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_i
        /// A wrapper that defers the instantiation of a template \p C with integral constant
        /// parameters \p Is in a \c lambda or \c let expression.
        /// \sa `defer`
        /// \ingroup invocation
        template <typename T, template <T...> class C, T... Is>
        struct defer_i : detail::defer_i_<T, C, Is...>
        {
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_trait
        /// A wrapper that defers the instantiation of a trait \p C with type parameters \p Ts in a
        /// \c lambda or \c let expression.
        /// \sa `defer`
        /// \ingroup invocation
        template <template <typename...> class C, typename... Ts>
        using defer_trait = defer<detail::_t_t, detail::defer_<C, Ts...>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // defer_trait_i
        /// A wrapper that defers the instantiation of a trait \p C with integral constant
        /// parameters \p Is in a \c lambda or \c let expression.
        /// \sa `defer_i`
        /// \ingroup invocation
        template <typename T, template <T...> class C, T... Is>
        using defer_trait_i = defer<detail::_t_t, detail::defer_i_<T, C, Is...>>;

        /// An alias that computes the size of the type \p T.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup trait
        template <class T>
        using sizeof_ = meta::size_t<sizeof(T)>;

        /// An alias that computes the alignment required for any instance of the type \p T.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup trait
        template <class T>
        using alignof_ = meta::size_t<alignof(T)>;

        namespace lazy
        {
            /// \sa `meta::sizeof_`
            /// \ingroup lazy_trait
            template <typename T>
            using sizeof_ = defer<sizeof_, T>;

            /// \sa `meta::alignof_`
            /// \ingroup lazy_trait
            template <typename T>
            using alignof_ = defer<alignof_, T>;
        }

        /// A Trait that always returns its argument \p T. It is also an Invocable
        /// that always returns \p T.
        /// \ingroup trait
        /// \ingroup invocation
        template <typename T>
        struct id
        {
            using type = T;

            template <typename...>
            using invoke = T;
        };

        /// An alias for type \p T. Useful in non-deduced contexts.
        /// \ingroup trait
        template <typename T>
        using id_t = _t<id<T>>;

        namespace lazy
        {
            /// \sa `meta::id`
            /// \ingroup lazy_trait
            /// \ingroup lazy_invocation
            template <typename T>
            using id = defer<id, T>;
        }

        /// is
        /// \cond
        namespace detail
        {
            template <typename, template <typename...> class>
            struct is_ : std::false_type
            {
            };

            template <typename... Ts, template <typename...> class C>
            struct is_<C<Ts...>, C> : std::true_type
            {
            };
        }
        /// \endcond

        /// Test whether a type \p T is an instantiation of class
        /// template \p C.
        /// \ingroup trait
        template <typename T, template <typename...> class C>
        using is = _t<detail::is_<T, C>>;

        /// Compose the Invocables \p Fns in the parameter pack \p Ts.
        /// \ingroup composition
        template <Invocable... Fns>
        struct compose
        {
        };

        template <Invocable Fn0>
        struct compose<Fn0>
        {
            template <typename... Ts>
            using invoke = invoke<Fn0, Ts...>;
        };

        template <Invocable Fn0, Invocable... Fns>
        struct compose<Fn0, Fns...>
        {
            template <typename... Ts>
            using invoke = invoke<Fn0, invoke<compose<Fns...>, Ts...>>;
        };

        namespace lazy
        {
            /// \sa 'meta::compose'
            /// \ingroup lazy_composition
            template <typename... Fns>
            using compose = defer<compose, Fns...>;
        }

        /// Turn a template \p C into an Invocable.
        /// \ingroup composition
        template <template <typename...> class C>
        struct quote
        {
            // Indirection through defer here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename... Ts>
            using invoke = _t<defer<C, Ts...>>;
        };

        /// Turn a template \p C taking literals of type \p T into a
        /// Invocable.
        /// \ingroup composition
        template <typename T, template <T...> class C>
        struct quote_i
        {
            // Indirection through defer_i here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <Integral... Ts>
            using invoke = _t<defer_i<T, C, _v<Ts>...>>;
        };

        /// Turn a trait template \p C into an Invocable.
        /// \ingroup composition
        template <template <typename...> class C>
        using quote_trait = compose<quote<_t>, quote<C>>;

        /// Turn a trait template \p C taking literals of type \p T into an Invocable.
        /// \ingroup composition
        template <typename T, template <T...> class C>
        using quote_trait_i = compose<quote<_t>, quote_i<T, C>>;

        /// A Invocable that partially applies the Invocable
        /// \p Fn by binding the arguments \p Ts to the \e front of \p Fn.
        /// \ingroup composition
        template <Invocable Fn, typename... Ts>
        struct bind_front
        {
            template <typename... Us>
            using invoke = invoke<Fn, Ts..., Us...>;
        };

        /// A Invocable that partially applies the Invocable \p Fn by binding the
        /// arguments \p Us to the \e back of \p Fn.
        /// \ingroup composition
        template <Invocable Fn, typename... Us>
        struct bind_back
        {
            template <typename... Ts>
            using invoke = invoke<Fn, Ts..., Us...>;
        };

        namespace lazy
        {
            /// \sa 'meta::bind_front'
            /// \ingroup lazy_composition
            template <typename Fn, typename... Ts>
            using bind_front = defer<bind_front, Fn, Ts...>;

            /// \sa 'meta::bind_back'
            /// \ingroup lazy_composition
            template <typename Fn, typename... Ts>
            using bind_back = defer<bind_back, Fn, Ts...>;
        }

        /// Extend meta with your own datatypes.
        namespace extension
        {
            /// A trait that unpacks the types in the type list \p L into the Invocable
            /// \p Fn.
            /// \ingroup extension
            template <Invocable Fn, typename L>
            struct apply
            {
            };

            template <Invocable Fn, typename Ret, typename... Args>
            struct apply<Fn, Ret(Args...)> : lazy::invoke<Fn, Ret, Args...>
            {
            };

            template <Invocable Fn, template <typename...> class T, typename... Ts>
            struct apply<Fn, T<Ts...>> : lazy::invoke<Fn, Ts...>
            {
            };

            template <Invocable Fn, typename T, T... Is>
            struct apply<Fn, integer_sequence<T, Is...>>
                : lazy::invoke<Fn, std::integral_constant<T, Is>...>
            {
            };
        }

        /// Applies the Invocable \p C using the types in the type list \p L as
        /// arguments.
        /// \ingroup invocation
        template <Invocable Fn, typename L>
        using apply = _t<extension::apply<Fn, L>>;

        namespace lazy
        {
            template <typename Fn, typename L>
            using apply = defer<apply, Fn, L>;
        }

        /// A Invocable that takes a bunch of arguments, bundles them into a type list, and
        /// then calls the Invocable \p Fn with the type list \p Q.
        /// \ingroup composition
        template <Invocable P, Invocable Q = quote<list>>
        using curry = compose<P, Q>;

        /// A Invocable that takes a type list, unpacks the types, and then calls the
        /// Invocable \p Fn with the types.
        /// \ingroup composition
        template <Invocable Fn>
        using uncurry = bind_front<quote<apply>, Fn>;

        namespace lazy
        {
            /// \sa 'meta::curry'
            /// \ingroup lazy_composition
            template <typename P, typename Q = quote<list>>
            using curry = defer<curry, P, Q>;

            /// \sa 'meta::uncurry'
            /// \ingroup lazy_composition
            template <typename Fn>
            using uncurry = defer<uncurry, Fn>;
        }

        /// A Invocable that reverses the order of the first two arguments.
        /// \ingroup composition
        template <Invocable Fn>
        struct flip
        {
        private:
            template <typename... Ts>
            struct impl
            {
            };
            template <typename A, typename B, typename... Ts>
            struct impl<A, B, Ts...> : lazy::invoke<Fn, B, A, Ts...>
            {
            };

        public:
            template <typename... Ts>
            using invoke = _t<impl<Ts...>>;
        };

        namespace lazy
        {
            /// \sa 'meta::flip'
            /// \ingroup lazy_composition
            template <typename Fn>
            using flip = defer<flip, Fn>;
        }

        /// \cond
        namespace detail
        {
            template <typename...>
            struct on_
            {
            };
            template <typename Fn, typename... Gs>
            struct on_<Fn, Gs...>
            {
                template <typename... Ts>
                using invoke = invoke<Fn, invoke<compose<Gs...>, Ts>...>;
            };
        }
        /// \endcond

        /// Use as `on<Fn, Gs...>`. Creates an Invocable that applies Invocable \c Fn to the
        /// result of applying Invocable `compose<Gs...>` to all the arguments.
        /// \ingroup composition
        template <Invocable... Fns>
        using on = detail::on_<Fns...>;

        namespace lazy
        {
            /// \sa 'meta::on'
            /// \ingroup lazy_composition
            template <typename Fn, typename G>
            using on = defer<on, Fn, G>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // if_
        /// \cond
        namespace detail
        {
            template <typename...>
            struct _if_
            {
            };

            template <Integral If>
            struct _if_<If> : std::enable_if<_v<If>>
            {
            };

            template <Integral If, typename Then>
            struct _if_<If, Then> : std::enable_if<_v<If>, Then>
            {
            };

            template <Integral If, typename Then, typename Else>
            struct _if_<If, Then, Else> : std::conditional<_v<If>, Then, Else>
            {
            };
        } // namespace detail
          /// \endcond

        /// Select one type or another depending on a compile-time Boolean.
        /// \ingroup logical
        template <typename... Args>
        using if_ = _t<detail::_if_<Args...>>;

        /// Select one type or another depending on a compile-time Boolean.
        /// \ingroup logical
        template <bool If, typename... Args>
        using if_c = _t<detail::_if_<bool_<If>, Args...>>;

        namespace lazy
        {
            /// \sa 'meta::if_'
            /// \ingroup lazy_logical
            template <typename... Args>
            using if_ = defer<if_, Args...>;

            /// \sa 'meta::if_c'
            /// \ingroup lazy_logical
            template <bool If, typename... Args>
            using if_c = if_<bool_<If>, Args...>;
        }

        /// \cond
        namespace detail
        {
            template <Integral... Bs>
            struct _and_ : std::true_type
            {
            };

            template <Integral B, Integral... Bs>
            requires _v<B> struct _and_<B, Bs...> : _and_<Bs...>
            {
            };

            template <Integral B, Integral... Bs>
            requires !_v<B> struct _and_<B, Bs...> : std::false_type
            {
            };

            template <Integral... Bs>
            struct _or_ : std::false_type
            {
            };

            template <Integral B, Integral... Bs>
            requires !_v<B> struct _or_<B, Bs...> : _or_<Bs...>
            {
            };

            template <Integral B, Integral... Bs>
            requires _v<B> struct _or_<B, Bs...> : std::true_type
            {
            };
        } // namespace detail
        /// \endcond

        /// Logically negate the Boolean parameter
        /// \ingroup logical
        template <bool B>
        using not_c = bool_<!B>;

        /// Logically negate the integral constant-wrapped Boolean parameter.
        /// \ingroup logical
        template <Integral B>
        using not_ = not_c<_v<B>>;

        /// Logically and together all the Boolean parameters
        /// \ingroup logical
        template <bool... Bs>
        using and_c = bool_<(true && ... && Bs)>;

        /// Logically and together all the integral constant-wrapped Boolean parameters, \e without
        /// doing short-circuiting.
        /// \ingroup logical
        template <Integral... Bs>
        using strict_and = and_c<_v<Bs>...>;

        /// Logically and together all the integral constant-wrapped Boolean parameters, \e with
        /// short-circuiting.
        /// \ingroup logical
        template <Integral... Bs>
        using and_ = _t<detail::_and_<Bs...>>;

        /// Logically or together all the Boolean parameters
        /// \ingroup logical
        template <bool... Bs>
        using or_c = bool_<(false || ... || Bs)>;

        /// Logically or together all the integral constant-wrapped Boolean parameters, \e without
        /// doing short-circuiting.
        /// \ingroup logical
        template <Integral... Bs>
        using strict_or = or_c<_v<Bs>...>;

        /// Logically or together all the integral constant-wrapped Boolean parameters, \e with
        /// short-circuiting.
        /// \ingroup logical
        template <Integral... Bs>
        using or_ = _t<detail::_or_<Bs...>>;

        namespace lazy
        {
            /// \sa 'meta::and_'
            /// \ingroup lazy_logical
            template <typename... Bs>
            using and_ = defer<and_, Bs...>;

            /// \sa 'meta::or_'
            /// \ingroup lazy_logical
            template <typename... Bs>
            using or_ = defer<or_, Bs...>;

            /// \sa 'meta::not_'
            /// \ingroup lazy_logical
            template <typename B>
            using not_ = defer<not_, B>;

            /// \sa 'meta::strict_and'
            /// \ingroup lazy_logical
            template <typename... Bs>
            using strict_and = defer<strict_and, Bs...>;

            /// \sa 'meta::strict_or'
            /// \ingroup lazy_logical
            template <typename... Bs>
            using strict_or = defer<strict_or, Bs...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // fold
        /// \cond
        namespace detail
        {
            template <typename, typename, typename>
            struct fold_
            {
            };

            template <typename State, typename Fn>
            struct fold_<list<>, State, Fn>
            {
                using type = State;
            };

            template <typename Head, typename... L, typename State, typename Fn>
            requires Valid<invoke, Fn, State, Head> struct fold_<list<Head, L...>, State, Fn>
                : fold_<list<L...>, invoke<Fn, State, Head>, Fn>
            {
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list constructed by doing a left fold of the list \p L using
        /// binary Invocable \p Fn and initial state \p State. That is, the \c State_N for
        /// the list element \c A_N is computed by `Fn(State_N-1, A_N) -> State_N`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L, typename State, Invocable Fn>
        using fold = _t<detail::fold_<L, State, Fn>>;

        /// An alias for `meta::fold`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L, typename State, Invocable Fn>
        using accumulate = fold<L, State, Fn>;

        namespace lazy
        {
            /// \sa 'meta::foldl'
            /// \ingroup lazy_transformation
            template <typename L, typename State, typename Fn>
            using fold = defer<fold, L, State, Fn>;

            /// \sa 'meta::accumulate'
            /// \ingroup lazy_transformation
            template <typename L, typename State, typename Fn>
            using accumulate = defer<accumulate, L, State, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse_fold
        /// \cond
        namespace detail
        {
            template <typename, typename, typename>
            struct reverse_fold_
            {
            };

            template <typename State, typename Fn>
            struct reverse_fold_<list<>, State, Fn>
            {
                using type = State;
            };

            template <typename Head, typename... L, typename State, typename Fn>
            requires Valid<_t, reverse_fold_<list<L...>, State, Fn>> struct reverse_fold_<
                list<Head, L...>, State, Fn>
                : lazy::invoke<Fn, _t<reverse_fold_<list<L...>, State, Fn>>, Head>
            {
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list constructed by doing a right fold of the list \p L using
        /// binary Invocable \p Fn and initial state \p State. That is, the \c State_N for
        /// the list element \c A_N is computed by `Fn(A_N, State_N+1) -> State_N`.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L, typename State, Invocable Fn>
        using reverse_fold = _t<detail::reverse_fold_<L, State, Fn>>;

        namespace lazy
        {
            /// \sa 'meta::foldr'
            /// \ingroup lazy_transformation
            template <typename L, typename State, typename Fn>
            using reverse_fold = defer<reverse_fold, L, State, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // npos
        /// A special value used to indicate no matches. It equals the maximum value representable
        /// by std::size_t.
        /// \ingroup list
        using npos = meta::size_t<std::size_t(-1)>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // list
        /// A list of types.
        /// \ingroup list
        template <typename... Ts>
        struct list
        {
            using type = list;
            /// \return `sizeof...(Ts)`
            static constexpr std::size_t size() noexcept { return sizeof...(Ts); }
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        // size
        /// An integral constant wrapper that is the size of the \c meta::list
        /// \p L.
        /// \ingroup list
        template <List L>
        using size = meta::size_t<L::size()>;

        namespace lazy
        {
            /// \sa 'meta::size'
            /// \ingroup lazy_list
            template <typename L>
            using size = defer<size, L>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // concat
        /// \cond
        namespace detail
        {
            template <typename... Lists>
            struct concat_
            {
            };

            template <>
            struct concat_<>
            {
                using type = list<>;
            };

            template <typename... L1>
            struct concat_<list<L1...>>
            {
                using type = list<L1...>;
            };

            template <typename... L1, typename... List2>
            struct concat_<list<L1...>, list<List2...>>
            {
                using type = list<L1..., List2...>;
            };

            template <typename... L1, typename... List2, typename... List3>
            struct concat_<list<L1...>, list<List2...>, list<List3...>>
            {
                using type = list<L1..., List2..., List3...>;
            };

            template <typename... L1, typename... List2, typename... List3, typename... Rest>
            struct concat_<list<L1...>, list<List2...>, list<List3...>, Rest...>
                : concat_<list<L1..., List2..., List3...>, Rest...>
            {
            };
        } // namespace detail
        /// \endcond

        /// Concatenates several lists into a single list.
        /// \pre The parameters must all be instantiations of \c meta::list.
        /// \par Complexity
        /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of lists.
        /// \ingroup transformation
        template <List... Ls>
        using concat = _t<detail::concat_<Ls...>>;

        namespace lazy
        {
            /// \sa 'meta::concat'
            /// \ingroup lazy_transformation
            template <typename... Lists>
            using concat = defer<concat, Lists...>;
        }

        /// Joins a list of lists into a single list.
        /// \pre The parameter must be an instantiation of \c meta::list\<T...\>
        /// where each \c T is itself an instantiation of \c meta::list.
        /// \par Complexity
        /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of
        /// lists.
        /// \ingroup transformation
        template <List ListOfLists>
        using join = apply<quote<concat>, ListOfLists>;

        namespace lazy
        {
            /// \sa 'meta::join'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using join = defer<join, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // repeat_n
        /// \cond
        namespace detail
        {
            template <std::size_t N, typename T>
            struct repeat_n_c_
            {
                using type = concat<_t<repeat_n_c_<N / 2, T>>, _t<repeat_n_c_<N / 2, T>>,
                                    _t<repeat_n_c_<N % 2, T>>>;
            };

            template <typename T>
            struct repeat_n_c_<0, T>
            {
                using type = list<>;
            };

            template <typename T>
            struct repeat_n_c_<1, T>
            {
                using type = list<T>;
            };
        } // namespace detail
        /// \endcond

        /// Generate `list<T,T,T...T>` of size \p N arguments.
        /// \par Complexity
        /// \f$ O(log N) \f$.
        /// \ingroup list
        template <Integral N, typename T = void>
        using repeat_n = _t<detail::repeat_n_c_<_v<N>, T>>;

        /// Generate `list<T,T,T...T>` of size \p N arguments.
        /// \par Complexity
        /// \f$ O(log N) \f$.
        /// \ingroup list
        template <std::size_t N, typename T = void>
        using repeat_n_c = _t<detail::repeat_n_c_<N, T>>;

        namespace lazy
        {
            /// \sa 'meta::repeat_n'
            /// \ingroup lazy_list
            template <typename N, typename T = void>
            using repeat_n = defer<repeat_n, N, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // at
        /// \cond
        namespace detail
        {
            template <typename VoidPtrs>
            struct at_impl_;

            template <typename... VoidPtrs>
            struct at_impl_<list<VoidPtrs...>>
            {
                static nil_ eval(...);

                template <typename T, typename... Us>
                static T eval(VoidPtrs..., T *, Us *...);
            };

            template <typename L, typename N>
            struct at_
            {
            };

            template <typename... Ts, typename N>
            struct at_<list<Ts...>, N>
                : decltype(at_impl_<repeat_n<N, void *>>::eval(detail::nullptr_v<id<Ts>>...))
            {
            };
        } // namespace detail
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // at
        /// Return the \p N th element in the \c meta::list \p L.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <List L, Integral N>
        using at = _t<detail::at_<L, N>>;

        /// Return the \p N th element in the \c meta::list \p L.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <List L, std::size_t N>
        using at_c = at<L, meta::size_t<N>>;

        namespace lazy
        {
            /// \sa 'meta::at'
            /// \ingroup lazy_list
            template <typename L, typename N>
            using at = defer<at, L, N>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // drop
        /// \cond
        namespace detail
        {
            ///////////////////////////////////////////////////////////////////////////////////////
            /// drop_impl_
            template <typename VoidPtrs>
            struct drop_impl_
            {
                static nil_ eval(...);
            };

            template <typename... VoidPtrs>
            struct drop_impl_<list<VoidPtrs...>>
            {
                static nil_ eval(...);

                template <typename... Ts>
                static id<list<Ts...>> eval(VoidPtrs..., id<Ts> *...);
            };

            template <>
            struct drop_impl_<list<>>
            {
                template <typename... Ts>
                static id<list<Ts...>> eval(id<Ts> *...);
            };

            template <typename L, typename N>
            struct drop_
            {
            };

            template <typename... Ts, typename N>
            struct drop_<list<Ts...>, N>
                : decltype(drop_impl_<repeat_n<N, void *>>::eval(detail::nullptr_v<id<Ts>>...))
            {
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by removing the first \p N elements from \p L.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <List L, Integral N>
        using drop = _t<detail::drop_<L, N>>;

        /// Return a new \c meta::list by removing the first \p N elements from \p L.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <List L, std::size_t N>
        using drop_c = _t<detail::drop_<L, meta::size_t<N>>>;

        namespace lazy
        {
            /// \sa 'meta::drop'
            /// \ingroup lazy_transformation
            template <typename L, typename N>
            using drop = defer<drop, L, N>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // front
        /// \cond
        namespace detail
        {
            template <typename L>
            struct front_
            {
            };

            template <typename Head, typename... L>
            struct front_<list<Head, L...>>
            {
                using type = Head;
            };
        } // namespace detail
        /// \endcond

        /// Return the first element in \c meta::list \p L.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup list
        template <List L>
        using front = _t<detail::front_<L>>;

        namespace lazy
        {
            /// \sa 'meta::front'
            /// \ingroup lazy_list
            template <typename L>
            using front = defer<front, L>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // back
        /// \cond
        namespace detail
        {
            template <typename L>
            struct back_
            {
            };

            template <typename Head, typename... L>
            struct back_<list<Head, L...>>
            {
                using type = at_c<list<Head, L...>, sizeof...(L)>;
            };
        } // namespace detail
        /// \endcond

        /// Return the last element in \c meta::list \p L.
        /// \par Complexity
        /// Amortized \f$ O(1) \f$.
        /// \ingroup list
        template <List L>
        using back = _t<detail::back_<L>>;

        namespace lazy
        {
            /// \sa 'meta::back'
            /// \ingroup lazy_list
            template <typename L>
            using back = defer<back, L>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // push_front
        /// Return a new \c meta::list by adding the element \c T to the front of \p L.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <List L, typename... Ts>
        using push_front = apply<bind_front<quote<list>, Ts...>, L>;

        namespace lazy
        {
            /// \sa 'meta::push_front'
            /// \ingroup lazy_transformation
            template <typename... Ts>
            using push_front = defer<push_front, Ts...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // pop_front
        /// \cond
        namespace detail
        {
            template <typename L>
            struct pop_front_
            {
            };

            template <typename Head, typename... L>
            struct pop_front_<list<Head, L...>>
            {
                using type = list<L...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list by removing the first element from the front of \p L.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup transformation
        template <List L>
        using pop_front = _t<detail::pop_front_<L>>;

        namespace lazy
        {
            /// \sa 'meta::pop_front'
            /// \ingroup lazy_transformation
            template <typename L>
            using pop_front = defer<pop_front, L>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // push_back
        /// Return a new \c meta::list by adding the element \c T to the back of \p L.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \note \c pop_back not provided because it cannot be made to meet the
        /// complexity guarantees one would expect.
        /// \ingroup transformation
        template <List L, typename... Ts>
        using push_back = apply<bind_back<quote<list>, Ts...>, L>;

        namespace lazy
        {
            /// \sa 'meta::push_back'
            /// \ingroup lazy_transformation
            template <typename... Ts>
            using push_back = defer<push_back, Ts...>;
        }

        /// \cond
        namespace detail
        {
            template <typename T, typename U>
            using min_ = if_<less<U, T>, U, T>;

            template <typename T, typename U>
            using max_ = if_<less<U, T>, T, U>;
        }
        /// \endcond

        /// An integral constant wrapper around the minimum of `Ts::type::value...`
        /// \ingroup math
        template <Integral... Ts>
        using min = fold<pop_front<list<Ts...>>, front<list<Ts...>>, quote<detail::min_>>;

        /// An integral constant wrapper around the maximum of `Ts::type::value...`
        /// \ingroup math
        template <Integral... Ts>
        using max = fold<pop_front<list<Ts...>>, front<list<Ts...>>, quote<detail::max_>>;

        namespace lazy
        {
            /// \sa 'meta::min'
            /// \ingroup lazy_math
            template <typename... Ts>
            using min = defer<min, Ts...>;

            /// \sa 'meta::max'
            /// \ingroup lazy_math
            template <typename... Ts>
            using max = defer<max, Ts...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // empty
        /// An Boolean integral constant wrapper around \c true if \p L is an empty type list; \c
        /// false, otherwise.
        /// \par Complexity
        /// \f$ O(1) \f$.
        /// \ingroup list
        template <List L>
        using empty = bool_<0 == _v<size<L>>>;

        namespace lazy
        {
            /// \sa 'meta::empty'
            /// \ingroup lazy_list
            template <typename L>
            using empty = defer<empty, L>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // pair
        /// A list with exactly two elements
        /// \ingroup list
        template <typename Fn, typename S>
        using pair = list<Fn, S>;

        /// Retrieve the first element of the \c pair \p Pair
        /// \ingroup list
        template <typename Pair>
        using first = front<Pair>;

        /// Retrieve the first element of the \c pair \p Pair
        /// \ingroup list
        template <typename Pair>
        using second = front<pop_front<Pair>>;

        namespace lazy
        {
            /// \sa 'meta::first'
            /// \ingroup lazy_list
            template <typename Pair>
            using first = defer<first, Pair>;

            /// \sa 'meta::second'
            /// \ingroup lazy_list
            template <typename Pair>
            using second = defer<second, Pair>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // find_index
        /// \cond
        namespace detail
        {
            // With thanks to Peter Dimov:
            constexpr std::size_t find_index_i_(bool const *const first, bool const *const last,
                                                std::size_t N = 0)
            {
                return first == last ? _v<npos> : *first ? N
                                                         : find_index_i_(first + 1, last, N + 1);
            }

            template <typename L, typename T>
            struct find_index_
            {
            };

            template <typename V>
            struct find_index_<list<>, V>
            {
                using type = npos;
            };

            template <typename... T, typename V>
            struct find_index_<list<T...>, V>
            {
                static constexpr bool s_v[] = {_v<std::is_same<T, V>>...};
                using type = size_t<find_index_i_(s_v, s_v + sizeof...(T))>;
            };
        } // namespace detail
        /// \endcond

        /// Fninds the index of the first occurrence of the type \p T within the list \p L.
        /// Returns `#meta::npos` if the type \p T was not found.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        /// \sa `meta::npos`
        template <List L, typename T>
        using find_index = _t<detail::find_index_<L, T>>;

        namespace lazy
        {
            /// \sa 'meta::find_index'
            /// \ingroup lazy_query
            template <typename L, typename T>
            using find_index = defer<find_index, L, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse_find_index
        /// \cond
        namespace detail
        {
            // With thanks to Peter Dimov:
            constexpr std::size_t reverse_find_index_i_(bool const *const first,
                                                        bool const *const last, std::size_t N)
            {
                return first == last ? _v<npos> : *(last - 1) ? N - 1 : reverse_find_index_i_(
                                                                            first, last - 1, N - 1);
            }

            template <typename L, typename T>
            struct reverse_find_index_
            {
            };

            template <typename V>
            struct reverse_find_index_<list<>, V>
            {
                using type = npos;
            };

            template <typename... T, typename V>
            struct reverse_find_index_<list<T...>, V>
            {
                static constexpr bool s_v[] = {_v<std::is_same<T, V>>...};
                using type = size_t<reverse_find_index_i_(s_v, s_v + sizeof...(T), sizeof...(T))>;
            };
        } // namespace detail
        /// \endcond

        /// Fninds the index of the last occurrence of the type \p T within the list \p L. Returns
        /// `#meta::npos` if the type \p T was not found.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        /// \sa `#meta::npos`
        template <List L, typename T>
        using reverse_find_index = _t<detail::reverse_find_index_<L, T>>;

        namespace lazy
        {
            /// \sa 'meta::reverse_find_index'
            /// \ingroup lazy_query
            template <typename L, typename T>
            using reverse_find_index = defer<reverse_find_index, L, T>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // reverse_find
        /// \cond
        namespace detail
        {
            template <typename L, typename T, typename State = list<>>
            struct reverse_find_
            {
            };

            template <typename T, typename State>
            struct reverse_find_<list<>, T, State>
            {
                using type = State;
            };

            template <typename Head, typename... L, typename T, typename State>
            struct reverse_find_<list<Head, L...>, T, State> : reverse_find_<list<L...>, T, State>
            {
            };

            template <typename... L, typename T, typename State>
            struct reverse_find_<list<T, L...>, T, State>
                : reverse_find_<list<L...>, T, list<T, L...>>
            {
            };
        }
        /// \endcond

        /// Return the tail of the list \p L starting at the first occurrence of \p T, if any
        /// such element exists; the empty list, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, typename T>
        using find = drop<L, min<find_index<L, T>, size<L>>>;

        namespace lazy
        {
            /// \sa 'meta::find'
            /// \ingroup lazy_query
            template <typename L, typename T>
            using find = defer<find, L, T>;
        }

        /// Return the tail of the list \p L starting at the last occurrence of \p T, if any such
        /// element exists; the empty list, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, typename T>
        using reverse_find = drop<L, min<reverse_find_index<L, T>, size<L>>>;

        namespace lazy
        {
            /// \sa 'meta::rfind'
            /// \ingroup lazy_query
            template <typename L, typename T>
            using reverse_find = defer<reverse_find, L, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // find_if
        /// \cond
        namespace detail
        {
            template <typename L, typename Fn>
            struct find_if_
            {
            };

            template <typename Fn>
            struct find_if_<list<>, Fn>
            {
                using type = list<>;
            };

            template <typename Head, typename... L, typename Fn>
            requires Integral<invoke<Fn, Head>> struct find_if_<list<Head, L...>, Fn>
                : if_<invoke<Fn, Head>, id<list<Head, L...>>, find_if_<list<L...>, Fn>>
            {
            };
        } // namespace detail
        /// \endcond

        /// Return the tail of the list \p L starting at the first element `A` such that
        /// `invoke<Fn, A>::%value` is \c true, if any such element exists; the empty list,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, Invocable Fn>
        using find_if = _t<detail::find_if_<L, Fn>>;

        namespace lazy
        {
            /// \sa 'meta::find_if'
            /// \ingroup lazy_query
            template <typename L, typename Fn>
            using find_if = defer<find_if, L, Fn>;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // reverse_find_if
        /// \cond
        namespace detail
        {
            template <typename L, typename Fn, typename State = list<>>
            struct reverse_find_if_
            {
            };

            template <typename Fn, typename State>
            struct reverse_find_if_<list<>, Fn, State>
            {
                using type = State;
            };

            template <typename Head, typename... L, typename Fn, typename State>
            requires Integral<invoke<Fn, Head>> struct reverse_find_if_<list<Head, L...>, Fn, State>
                : reverse_find_if_<list<L...>, Fn, if_<invoke<Fn, Head>, list<Head, L...>, State>>
            {
            };
        }
        /// \endcond

        /// Return the tail of the list \p L starting at the last element `A` such that
        /// `invoke<Fn, A>::%value` is \c true, if any such element exists; the empty list,
        /// otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, Invocable Fn>
        using reverse_find_if = _t<detail::reverse_find_if_<L, Fn>>;

        namespace lazy
        {
            /// \sa 'meta::rfind_if'
            /// \ingroup lazy_query
            template <typename L, typename Fn>
            using reverse_find_if = defer<reverse_find_if, L, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // replace
        /// \cond
        namespace detail
        {
            template <typename L, typename T, typename U>
            struct replace_
            {
            };

            template <typename... L, typename T, typename U>
            struct replace_<list<L...>, T, U>
            {
                using type = list<if_<std::is_same<T, L>, U, L>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list where all instances of type \p T have been replaced with
        /// \p U.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L, typename T, typename U>
        using replace = _t<detail::replace_<L, T, U>>;

        namespace lazy
        {
            /// \sa 'meta::replace'
            /// \ingroup lazy_transformation
            template <typename L, typename T, typename U>
            using replace = defer<replace, T, U>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // replace_if
        /// \cond
        namespace detail
        {
            template <typename L, typename C, typename U>
            struct replace_if_
            {
            };

            template <typename... L, typename C, typename U>
            requires(Integral<invoke<C, L>> && ...) struct replace_if_<list<L...>, C, U>
            {
                using type = list<if_<invoke<C, L>, U, L>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list where all elements \c A of the list \p L for which
        /// `invoke<C,A>::%value` is \c true have been replaced with \p U.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L, typename C, typename U>
        using replace_if = _t<detail::replace_if_<L, C, U>>;

        namespace lazy
        {
            /// \sa 'meta::replace_if'
            /// \ingroup lazy_transformation
            template <typename L, typename C, typename U>
            using replace_if = defer<replace_if, C, U>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        // count
        namespace detail
        {
            template <typename, typename>
            struct count_
            {
            };

            template <typename... Ts, typename T>
            struct count_<list<Ts...>, T>
            {
                using type = meta::size_t<((std::size_t)_v<std::is_same<T, Ts>> +...)>;
            };
        }

        /// Count the number of times a type \p T appears in the list \p L.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, typename T>
        using count = _t<detail::count_<L, T>>;

        namespace lazy
        {
            /// \sa `meta::count`
            /// \ingroup lazy_query
            template <typename L, typename T>
            using count = defer<count, L, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        // count_if
        namespace detail
        {
            template <typename, typename>
            struct count_if_
            {
            };

            template <typename... Ts, typename Fn>
            requires(Integral<invoke<Fn, Ts>> &&...) struct count_if_<list<Ts...>, Fn>
            {
                using type = meta::size_t<((std::size_t)(bool)_v<invoke<Fn, Ts>> +...)>;
            };
        }

        /// Count the number of times the predicate \p Fn evaluates to true for all the elements in
        /// the list \p L.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, Invocable Fn>
        using count_if = _t<detail::count_if_<L, Fn>>;

        namespace lazy
        {
            /// \sa `meta::count_if`
            /// \ingroup lazy_query
            template <typename L, typename Fn>
            using count_if = defer<count_if, L, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // transform
        /// \cond
        namespace detail
        {
            template <typename... Args>
            struct transform_
            {
            };

            template <typename... Ts, Invocable Fn>
            requires(Valid<invoke, Fn, Ts> &&...) struct transform_<list<Ts...>, Fn>
            {
                using type = list<invoke<Fn, Ts>...>;
            };

            template <typename... Ts, typename... Us, Invocable Fn>
            requires(Valid<invoke, Fn, Ts, Us> &&...) struct transform_<list<Ts...>, list<Us...>, Fn>
            {
                using type = list<invoke<Fn, Ts, Us>...>;
            };
        } // namespace detail
        /// \endcond

        /// Return a new \c meta::list constructed by transforming all the elements in \p L with
        /// the unary Invocable \p Fn. \c transform can also be called with two lists of
        /// the same length and a binary Invocable, in which case it returns a new list
        /// constructed with the results of calling \c Fn with each element in the lists,
        /// pairwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <typename... Args>
        using transform = _t<detail::transform_<Args...>>;

        namespace lazy
        {
            /// \sa 'meta::transform'
            /// \ingroup lazy_transformation
            template <typename... Args>
            using transform = defer<transform, Args...>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // filter
        /// \cond
        namespace detail
        {
            template <typename Fn>
            struct filter_
            {
                template <typename State, typename A>
                using invoke = if_<invoke<Fn, A>, push_back<State, A>, State>;
            };
        } // namespace detail
        /// \endcond

        /// Returns a new meta::list where only those elements of \p L that satisfy the
        /// Invocable \p Fn such that `invoke<Fn,A>::%value` is \c true are present.
        /// That is, those elements that don't satisfy the \p Fn are "removed".
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L, Invocable Fn>
        using filter = fold<L, list<>, detail::filter_<Fn>>;

        namespace lazy
        {
            /// \sa 'meta::filter'
            /// \ingroup lazy_transformation
            template <typename L, typename Fn>
            using filter = defer<filter, L, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // for_each
        /// \cond
        namespace detail
        {
            struct for_each_fn
            {
                template <class Fn, class... Args>
                constexpr auto operator()(list<Args...>, Fn f) const -> Fn
                {
                    return (void)std::initializer_list<int>{((void)f(Args{}), 0)...}, f;
                }
            };
        } // namespace detail
        /// \endcond

        /// `for_each(L, Fn)` calls the \p Fn for each
        /// argument in the \p L.
        /// \ingroup runtime
        inline constexpr detail::for_each_fn for_each {};

        ///////////////////////////////////////////////////////////////////////////////////////////
        // transpose
        /// Given a list of lists of types \p ListOfLists, transpose the elements from the lists.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <List ListOfLists>
        using transpose = fold<ListOfLists, repeat_n<size<front<ListOfLists>>, list<>>,
                               bind_back<quote<transform>, quote<push_back>>>;

        namespace lazy
        {
            /// \sa 'meta::transpose'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using transpose = defer<transpose, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // zip_with
        /// Given a list of lists of types \p ListOfLists and an Invocable \p Fn, construct
        /// a new list by calling \p Fn with the elements from the lists pairwise.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <Invocable Fn, List ListOfLists>
        using zip_with = transform<transpose<ListOfLists>, uncurry<Fn>>;

        namespace lazy
        {
            /// \sa 'meta::zip_with'
            /// \ingroup lazy_transformation
            template <typename Fn, typename ListOfLists>
            using zip_with = defer<zip_with, Fn, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // zip
        /// Given a list of lists of types \p ListOfLists, construct a new list by grouping the
        /// elements from the lists pairwise into `meta::list`s.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and \f$ M \f$
        /// is the size of the inner lists.
        /// \ingroup transformation
        template <List ListOfLists>
        using zip = transpose<ListOfLists>;

        namespace lazy
        {
            /// \sa 'meta::zip'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using zip = defer<zip, ListOfLists>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // as_list
        /// \cond
        namespace detail
        {
            template <typename T>
            using uncvref_t = _t<std::remove_cv<_t<std::remove_reference<T>>>>;

            // Indirection here needed to avoid Core issue 1430
            // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
            template <typename Sequence>
            struct as_list_ : lazy::invoke<uncurry<curry<quote_trait<id>>>, uncvref_t<Sequence>>
            {
            };
        } // namespace detail
        /// \endcond

        /// Turn a type into an instance of \c meta::list in a way determined by
        /// \c meta::apply.
        /// \ingroup list
        template <typename Sequence>
        using as_list = _t<detail::as_list_<Sequence>>;

        namespace lazy
        {
            /// \sa 'meta::as_list'
            /// \ingroup lazy_list
            template <typename Sequence>
            using as_list = defer<as_list, Sequence>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // reverse
        /// Return a new \c meta::list by reversing the elements in the list \p L.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L>
        using reverse = reverse_fold<L, list<>, quote<push_back>>;

        namespace lazy
        {
            /// \sa 'meta::reverse'
            /// \ingroup lazy_transformation
            template <typename L>
            using reverse = defer<reverse, L>;
        }

        /// Logically negate the result of Invocable \p Fn.
        /// \ingroup trait
        template <Invocable Fn>
        using not_fn = compose<quote<not_>, Fn>;

        namespace lazy
        {
            /// \sa 'meta::not_fn'
            /// \ingroup lazy_trait
            template <typename Fn>
            using not_fn = defer<not_fn, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // all_of
        /// A Boolean integral constant wrapper around \c true if `invoke<Fn, A>::%value` is \c true
        /// for all elements \c A in \c meta::list \p L; \c false, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, Invocable Fn>
        using all_of = empty<find_if<L, not_fn<Fn>>>;

        namespace lazy
        {
            /// \sa 'meta::all_of'
            /// \ingroup lazy_query
            template <typename L, typename Fn>
            using all_of = defer<all_of, L, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // any_of
        /// A Boolean integral constant wrapper around \c true if `invoke<Fn, A>::%value` is
        /// \c true for any element \c A in \c meta::list \p L; \c false, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, Invocable Fn>
        using any_of = not_<empty<find_if<L, Fn>>>;

        namespace lazy
        {
            /// \sa 'meta::any_of'
            /// \ingroup lazy_query
            template <typename L, typename Fn>
            using any_of = defer<any_of, L, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // none_of
        /// A Boolean integral constant wrapper around \c true if `invoke<Fn, A>::%value` is
        /// \c false for all elements \c A in \c meta::list \p L; \c false, otherwise.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, Invocable Fn>
        using none_of = empty<find_if<L, Fn>>;

        namespace lazy
        {
            /// \sa 'meta::none_of'
            /// \ingroup lazy_query
            template <typename L, Invocable Fn>
            using none_of = defer<none_of, L, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // in
        /// A Boolean integral constant wrapper around \c true if there is at least one occurrence
        /// of \p T in \p L.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup query
        template <List L, typename T>
        using in = not_<empty<find<L, T>>>;

        namespace lazy
        {
            /// \sa 'meta::in'
            /// \ingroup lazy_query
            template <typename L, typename T>
            using in = defer<in, L, T>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // inherit
        /// \cond
        namespace detail
        {
            template <typename L>
            struct inherit_
            {
            };

            template <typename... L>
            struct inherit_<list<L...>> : L...
            {
                using type = inherit_;
            };
        }
        /// \endcond

        /// A type that inherits from all the types in the list
        /// \pre The types in the list must be unique
        /// \pre All the types in the list must be non-final class types
        /// \ingroup datatype
        template <List L>
        using inherit = meta::_t<detail::inherit_<L>>;

        namespace lazy
        {
            /// \sa 'meta::inherit'
            /// \ingroup lazy_datatype
            template <typename L>
            using inherit = defer<inherit, L>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // set
        // Used to improve the performance of \c meta::unique.
        /// \cond
        namespace detail
        {
            template <typename Set, typename T>
            struct in_
            {
            };

            template <typename... Set, typename T>
            struct in_<list<Set...>, T> : std::is_base_of<id<T>, inherit<list<id<Set>...>>>
            {
            };

            template <typename Set, typename T>
            struct insert_back_
            {
            };

            template <typename... Set, typename T>
            struct insert_back_<list<Set...>, T>
            {
                using type = if_<in_<list<Set...>, T>, list<Set...>, list<Set..., T>>;
            };
        } // namespace detail
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // unique
        /// Return a new \c meta::list where all duplicate elements have been removed.
        /// \par Complexity
        /// \f$ O(N^2) \f$.
        /// \ingroup transformation
        template <List L>
        using unique = fold<L, list<>, quote_trait<detail::insert_back_>>;

        namespace lazy
        {
            /// \sa 'meta::unique'
            /// \ingroup lazy_transformation
            template <typename L>
            using unique = defer<unique, L>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // partition
        /// \cond
        namespace detail
        {
            template <typename Fn>
            struct partition_
            {
                template <typename, typename>
                struct impl
                {
                };
                template <typename... Yes, typename... No, typename A>
                requires Integral<invoke<Fn, A>> struct impl<pair<list<Yes...>, list<No...>>, A>
                {
                    using type = if_<invoke<Fn, A>, pair<list<Yes..., A>, list<No...>>,
                                     pair<list<Yes...>, list<No..., A>>>;
                };
                template <typename State, typename A>
                using invoke = _t<impl<State, A>>;
            };
        } // namespace detail
        /// \endcond

        /// Returns a pair of lists, where the elements of \p L that satisfy the
        /// Invocable \p Fn such that `invoke<Fn,A>::%value` is \c true are present in the
        /// first list and the rest are in the second.
        /// \par Complexity
        /// \f$ O(N) \f$.
        /// \ingroup transformation
        template <List L, Invocable Fn>
        using partition = fold<L, pair<list<>, list<>>, detail::partition_<Fn>>;

        namespace lazy
        {
            /// \sa 'meta::partition'
            /// \ingroup lazy_transformation
            template <typename L, typename Fn>
            using partition = defer<partition, L, Fn>;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // sort
        /// \cond
        namespace detail
        {
            template <Invocable Fn, typename A, typename B, typename... Ts>
            using part_ = partition<list<B, Ts...>, bind_back<Fn, A>>;

            template <List L, Invocable Fn>
            struct sort_
            {
            };

            template <typename Fn>
            struct sort_<list<>, Fn>
            {
                using type = list<>;
            };

            template <typename A, typename Fn>
            struct sort_<list<A>, Fn>
            {
                using type = list<A>;
            };

            template <typename A, typename B, typename... Ts, typename Fn>
            requires Valid<_t, sort_<first<part_<Fn, A, B, Ts...>>, Fn>> &&
                     Valid<_t, sort_<second<part_<Fn, A, B, Ts...>>, Fn>>
            struct sort_<list<A, B, Ts...>, Fn>
            {
                using P = part_<Fn, A, B, Ts...>;
                using type = concat<_t<sort_<first<P>, Fn>>, list<A>, _t<sort_<second<P>, Fn>>>;
            };
        }
        /// \endcond

        // clang-format off
        /// Return a new \c meta::list that is sorted according to Invocable predicate \p Fn.
        /// \par Complexity
        /// Expected: \f$ O(N log N) \f$
        /// Worst case: \f$ O(N^2) \f$.
        /// \code
        /// using L0 = list<char[5], char[3], char[2], char[6], char[1], char[5], char[10]>;
        /// using L1 = meta::sort<L0, lambda<_a, _b, lazy::less<lazy::sizeof_<_a>, lazy::sizeof_<_b>>>>;
        /// static_assert(std::is_same<L1, list<char[1], char[2], char[3], char[5], char[5], char[6], char[10]>>::value, "");
        /// \endcode
        /// \ingroup transformation
        // clang-format on
        template <List L, Invocable Fn>
        using sort = _t<detail::sort_<L, Fn>>;

        namespace lazy
        {
            /// \sa 'meta::sort'
            /// \ingroup lazy_transformation
            template <typename L, typename Fn>
            using sort = defer<sort, L, Fn>;
        }

        ////////////////////////////////////////////////////////////////////////////
        // lambda_
        /// \cond
        namespace detail
        {
            template <typename T, int = 0>
            struct protect_;

            template <typename, int = 0>
            struct vararg_;

            template <typename T, int = 0>
            struct is_valid_;

            // Returns which branch to evaluate
            template <typename If, typename... Ts>
            using lazy_if_ = lazy::_t<defer<_if_, If, protect_<Ts>...>>;

            template <typename A, typename T, typename Fn, typename Ts>
            struct subst1_
            {
                using type = list<list<T>>;
            };
            template <typename T, typename Fn, typename Ts>
            struct subst1_<Fn, T, Fn, Ts>
            {
                using type = list<>;
            };
            template <typename A, typename T, typename Fn, typename Ts>
            struct subst1_<vararg_<A>, T, Fn, Ts>
            {
                using type = list<Ts>;
            };

            template <List As, List Ts>
                requires _v<size<Ts>> + 2 >= _v<size<As>>
            using substitutions = push_back<
                join<transform<
                    concat<As, repeat_n_c<size<Ts>{} + 2 - size<As>{}, back<As>>>,
                    concat<Ts, repeat_n_c<2, back<As>>>,
                    bind_back<quote_trait<subst1_>, back<As>, drop_c<Ts, size<As>{} - 2>>>>,
                list<back<As>>>;

            template <typename T>
            struct is_vararg_ : std::false_type
            {
            };
            template <typename T>
            struct is_vararg_<vararg_<T>> : std::true_type
            {
            };

            template <List Tags>
            using is_variadic_ = is_vararg_<at<push_front<Tags, void>, dec<size<Tags>>>>;

            template <List Tags, bool IsVariadic = _v<is_variadic_<Tags>>>
            struct lambda_;

            // Non-variadic lambda implementation
            template <typename... As>
            struct lambda_<list<As...>, false>
            {
            private:
                static constexpr std::size_t arity = sizeof...(As)-1;
                using Tags = list<As...>; // Includes the lambda body as the last arg!
                using Fn = back<Tags>;
                template <typename T, List Args>
                struct impl;
                template <typename T, List Args>
                using lazy_impl_ = lazy::_t<defer<impl, T, protect_<Args>>>;
                template <typename, List>
                struct subst_
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                requires Valid<C, _t<impl<Ts, Args>>...> struct subst_<defer<C, Ts...>, Args>
                {
                    using type = C<_t<impl<Ts, Args>>...>;
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                requires Valid_I<T, C, Is...> struct subst_<defer_i<T, C, Is...>, Args>
                {
                    using type = C<Is...>;
                };
                template <typename T, List Args>
                struct impl : if_c<(reverse_find_index<Tags, T>() != npos()),
                                   lazy::at<Args, reverse_find_index<Tags, T>>, id<T>>
                {
                };
                template <typename T, typename Args>
                struct impl<protect_<T>, Args>
                {
                    using type = T;
                };
                template <typename T, typename Args>
                struct impl<is_valid_<T>, Args>
                {
                    using type = is_trait<impl<T, Args>>;
                };
                template <typename If, typename... Ts, typename Args>
                struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                    : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                {
                };
                template <typename B, typename... Bs, typename Args>
                struct impl<defer<and_, B, Bs...>, Args> // Short-circuit and_
                    : impl<lazy_impl_<lazy_if_<B, lazy::and_<Bs...>, protect_<std::false_type>>,
                                      Args>,
                           Args>
                {
                };
                template <typename B, typename... Bs, typename Args>
                struct impl<defer<or_, B, Bs...>, Args> // Short-circuit or_
                    : impl<
                          lazy_impl_<lazy_if_<B, protect_<std::true_type>, lazy::or_<Bs...>>, Args>,
                          Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename... Ts, typename Args>
                struct impl<lambda_<list<Ts...>, false>, Args>
                {
                    using type = compose<uncurry<lambda_<list<As..., Ts...>, false>>,
                                         curry<bind_front<quote<concat>, Args>>>;
                };
                template <typename... Bs, typename Args>
                struct impl<lambda_<list<Bs...>, true>, Args>
                {
                    using type = compose<typename lambda_<list<As..., Bs...>, true>::thunk,
                                         bind_front<quote<concat>, transform<Args, quote<list>>>,
                                         curry<bind_front<quote<substitutions>, list<Bs...>>>>;
                };

            public:
                template <typename... Ts>
                    requires sizeof...(Ts) == arity
                using invoke = _t<impl<Fn, list<Ts..., Fn>>>;
            };

            // Lambda with variadic placeholder (broken out due to less efficient compile-time
            // resource usage)
            template <typename... As>
            struct lambda_<list<As...>, true>
            {
            private:
                template <typename T, bool IsVar>
                friend struct lambda_;
                using Tags = list<As...>; // Includes the lambda body as the last arg!
                template <typename T, List Args>
                struct impl;
                template <List Args>
                using eval_impl_ = bind_back<quote_trait<impl>, Args>;
                template <typename T, List Args>
                using lazy_impl_ = lazy::_t<defer<impl, T, protect_<Args>>>;
                template <template <typename...> class C, List Args, List Ts>
                using try_subst_ = apply<quote<C>, join<transform<Ts, eval_impl_<Args>>>>;
                template <typename, List>
                struct subst_
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                requires
                    True<try_subst_<C, Args, list<Ts...>>> struct subst_<defer<C, Ts...>, Args>
                {
                    using type = list<try_subst_<C, Args, list<Ts...>>>;
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                requires Valid_I<T, C, Is...> struct subst_<defer_i<T, C, Is...>, Args>
                {
                    using type = list<C<Is...>>;
                };
                template <typename T, List Args>
                struct impl : if_c<(reverse_find_index<Tags, T>() != npos()),
                                   lazy::at<Args, reverse_find_index<Tags, T>>, id<list<T>>>
                {
                };
                template <typename T, typename Args>
                struct impl<protect_<T>, Args>
                {
                    using type = list<T>;
                };
                template <typename T, typename Args>
                struct impl<is_valid_<T>, Args>
                {
                    using type = list<is_trait<impl<T, Args>>>;
                };
                template <typename If, typename... Ts, typename Args>
                struct impl<defer<if_, If, Ts...>, Args> // Short-circuit if_
                    : impl<lazy_impl_<lazy_if_<If, Ts...>, Args>, Args>
                {
                };
                template <typename B, typename... Bs, typename Args>
                struct impl<defer<and_, B, Bs...>, Args> // Short-circuit and_
                    : impl<lazy_impl_<lazy_if_<B, lazy::and_<Bs...>, protect_<std::false_type>>,
                                      Args>,
                           Args>
                {
                };
                template <typename B, typename... Bs, typename Args>
                struct impl<defer<or_, B, Bs...>, Args> // Short-circuit or_
                    : impl<
                          lazy_impl_<lazy_if_<B, protect_<std::true_type>, lazy::or_<Bs...>>, Args>,
                          Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<defer<C, Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename T, template <T...> class C, T... Is, typename Args>
                struct impl<defer_i<T, C, Is...>, Args> : subst_<defer_i<T, C, Is...>, Args>
                {
                };
                template <template <typename...> class C, typename... Ts, typename Args>
                struct impl<C<Ts...>, Args> : subst_<defer<C, Ts...>, Args>
                {
                };
                template <typename... Bs, bool IsVar, typename Args>
                struct impl<lambda_<list<Bs...>, IsVar>, Args>
                {
                    using type =
                        list<compose<typename lambda_<list<As..., Bs...>, true>::thunk,
                                     bind_front<quote<concat>, Args>,
                                     curry<bind_front<quote<substitutions>, list<Bs...>>>>>;
                };
                struct thunk
                {
                    template <typename S, typename R = _t<impl<back<Tags>, S>>>
                        requires _v<size<R>> == 1
                    using invoke = front<R>;
                };

            public:
                template <typename... Ts>
                using invoke = invoke<thunk, substitutions<Tags, list<Ts...>>>;
            };
        }
        /// \endcond

        ///////////////////////////////////////////////////////////////////////////////////////////
        // lambda
        /// For creating anonymous Invocables.
        /// \code
        /// using L = lambda<_a, _b, std::pair<_b, std::pair<_a, _a>>>;
        /// using P = invoke<L, int, short>;
        /// static_assert(std::is_same<P, std::pair<short, std::pair<int, int>>>::value, "");
        /// \endcode
        /// \ingroup trait
        template <typename... Ts>
            requires sizeof...(Ts) > 0
        using lambda = detail::lambda_<list<Ts...>>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // is_valid
        /// For testing whether a deferred computation will succeed in a \c let or a \c lambda.
        /// \ingroup trait
        template <typename T>
        using is_valid = detail::is_valid_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // vararg
        /// For defining variadic placeholders.
        template <typename T>
        using vararg = detail::vararg_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // protect
        /// For preventing the evaluation of a nested `defer`ed computation in a \c let or
        /// \c lambda expression.
        template <typename T>
        using protect = detail::protect_<T>;

        ///////////////////////////////////////////////////////////////////////////////////////////
        // var
        /// For use when defining local variables in \c meta::let expressions
        /// \sa `meta::let`
        template <typename Tag, typename Value>
        struct var;

        /// \cond
        namespace detail
        {
            template <typename...>
            struct let_
            {
            };
            template <typename Fn>
            struct let_<Fn>
            {
                using type = lazy::invoke<lambda<Fn>>;
            };
            template <typename Tag, typename Value, typename... Rest>
            struct let_<var<Tag, Value>, Rest...>
            {
                using type = lazy::invoke<lambda<Tag, _t<let_<Rest...>>>, Value>;
            };
        }
        /// \endcond

        /// A lexically scoped expression with local variables.
        ///
        /// \code
        /// template <typename T, typename L>
        /// using find_index_ = let<
        ///     var<_a, L>,
        ///     var<_b, lazy::find<_a, T>>,
        ///     lazy::if_<
        ///         std::is_same<_b, list<>>,
        ///         meta::npos,
        ///         lazy::minus<lazy::size<_a>, lazy::size<_b>>>>;
        /// static_assert(find_index_<int, list<short, int, float>>{} == 1, "");
        /// static_assert(find_index_<double, list<short, int, float>>{} == meta::npos{}, "");
        /// \endcode
        /// \ingroup trait
        template <typename... As>
        using let = _t<_t<detail::let_<As...>>>;

        namespace lazy
        {
            /// \sa `meta::let`
            /// \ingroup lazy_trait
            template <typename... As>
            using let = defer<let, As...>;
        }

        // Some argument placeholders for use in \c lambda expressions.
        /// \ingroup trait
        inline namespace placeholders
        {
            // regular placeholders:
            struct _a;
            struct _b;
            struct _c;
            struct _d;
            struct _e;
            struct _f;
            struct _g;
            struct _h;
            struct _i;

            // variadic placeholders:
            using _args = vararg<void>;
            using _args_a = vararg<_a>;
            using _args_b = vararg<_b>;
            using _args_c = vararg<_c>;
        } // namespace placeholders

        ///////////////////////////////////////////////////////////////////////////////////////////
        // cartesian_product
        /// \cond
        namespace detail
        {
            template <typename M2, typename M>
            struct cartesian_product_fn
            {
                template <typename X>
                struct lambda0
                {
                    template <typename Xs>
                    using lambda1 = list<push_front<Xs, X>>;
                    using type = join<transform<M2, quote<lambda1>>>;
                };
                using type = join<transform<M, quote_trait<lambda0>>>;
            };
        } // namespace detail
        /// \endcond

        /// Given a list of lists \p ListOfLists, return a new list of lists that is the Cartesian
        /// Product. Like the `sequence` function from the Haskell Prelude.
        /// \par Complexity
        /// \f$ O(N \times M) \f$, where \f$ N \f$ is the size of the outer list, and
        /// \f$ M \f$ is the size of the inner lists.
        /// \ingroup transformation
        template <List ListOfLists>
        using cartesian_product =
            reverse_fold<ListOfLists, list<list<>>, quote_trait<detail::cartesian_product_fn>>;

        namespace lazy
        {
            /// \sa 'meta::cartesian_product'
            /// \ingroup lazy_transformation
            template <typename ListOfLists>
            using cartesian_product = defer<cartesian_product, ListOfLists>;
        }

        /// \cond
        ///////////////////////////////////////////////////////////////////////////////////////////
        // add_const_if
        template <typename If>
        using add_const_if = if_<If, quote_trait<std::add_const>, quote_trait<id>>;

        template <bool If>
        using add_const_if_c = if_c<If, quote_trait<std::add_const>, quote_trait<id>>;
        /// \endcond

        /// \cond
        namespace detail
        {
            // Glue two sets of integer_sequence together
            template <typename I1, typename I2, typename I3>
            struct integer_sequence_cat;

            template <typename T, T... N1, T... N2, T... N3>
            struct integer_sequence_cat<integer_sequence<T, N1...>, integer_sequence<T, N2...>,
                                        integer_sequence<T, N3...>>
            {
                using type = integer_sequence<T, N1..., (sizeof...(N1) + N2)...,
                                              (sizeof...(N1) + sizeof...(N2) + N3)...>;
            };

            template <typename T, std::size_t N>
            struct make_integer_sequence_
                : integer_sequence_cat<_t<make_integer_sequence_<T, N / 2>>,
                                       _t<make_integer_sequence_<T, N / 2>>,
                                       _t<make_integer_sequence_<T, N % 2>>>
            {
            };

            template <typename T>
            struct make_integer_sequence_<T, 0>
            {
                using type = integer_sequence<T>;
            };

            template <typename T>
            struct make_integer_sequence_<T, 1>
            {
                using type = integer_sequence<T, 0>;
            };
        } // namespace detail
        /// \endcond

        /// Generate \c integer_sequence containing integer constants [0,1,2,...,N-1].
        /// \par Complexity
        /// \f$ O(log(N)) \f$.
        /// \ingroup integral
        template <typename T, T N>
        using make_integer_sequence = _t<detail::make_integer_sequence_<T, (std::size_t)N>>;

        /// A container for a sequence of compile-time integer constants of type
        /// \c std::size_t
        /// \ingroup integral
        template <std::size_t... Is>
        using index_sequence = integer_sequence<std::size_t, Is...>;

        /// Generate \c index_sequence containing integer constants [0,1,2,...,N-1].
        /// \par Complexity
        /// \f$ O(log(N)) \f$.
        /// \ingroup integral
        template <std::size_t N>
        using make_index_sequence = make_integer_sequence<std::size_t, N>;

        /// \cond
        namespace detail
        {
            template <typename State, typename Ch>
            using atoi_ = if_c<(_v<Ch> >= '0' && _v<Ch> <= '9'),
                               std::integral_constant<typename State::value_type,
                                                      _v<State> * 10 + (_v<Ch> - '0')>>;
        }
        /// \endcond

        inline namespace literals
        {
            /// A user-defined literal that generates objects of type \c meta::size_t.
            /// \ingroup integral
            template <char... Chs>
            constexpr fold<list<char_<Chs>...>, meta::size_t<0>, quote<detail::atoi_>> operator"" _z()
            {
                return {};
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // integer_range
        /// \cond
        namespace detail
        {
            template <class T, T offset, class U>
            struct offset_integer_sequence_
            {
            };

            template <class T, T offset, T... Ts>
            struct offset_integer_sequence_<T, offset, meta::integer_sequence<T, Ts...>>
            {
                using type = meta::integer_sequence<T, (Ts + offset)...>;
            };
        } // namespace detail
        /// \endcond

        /// Makes the integer sequence [from, to).
        /// \ingroup integral
        template <class T, T from, T to>
        using integer_range = meta::_t<
            detail::offset_integer_sequence_<T, from, meta::make_integer_sequence<T, to - from>>>;
        /// \cond
    } // namespace v1
    /// \endcond
} // namespace meta

/// \cond
#if defined(__clang__) && defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 1101

_LIBCPP_BEGIN_NAMESPACE_STD
template <class>
class _LIBCPP_TYPE_VIS_ONLY allocator;
template <class, class>
struct _LIBCPP_TYPE_VIS_ONLY pair;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY hash;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY less;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY equal_to;
template <class>
struct _LIBCPP_TYPE_VIS_ONLY char_traits;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY list;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY forward_list;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY vector;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY deque;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY basic_string;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY map;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY multimap;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY set;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY multiset;
template <class, class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_map;
template <class, class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_multimap;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_set;
template <class, class, class, class>
class _LIBCPP_TYPE_VIS_ONLY unordered_multiset;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY queue;
template <class, class, class>
class _LIBCPP_TYPE_VIS_ONLY priority_queue;
template <class, class>
class _LIBCPP_TYPE_VIS_ONLY stack;
_LIBCPP_END_NAMESPACE_STD

namespace meta
{
    inline namespace v1
    {
        namespace detail
        {
            template <typename T, typename A = std::allocator<T>>
            using std_list = std::list<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_forward_list = std::forward_list<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_vector = std::vector<T, A>;
            template <typename T, typename A = std::allocator<T>>
            using std_deque = std::deque<T, A>;
            template <typename T, typename C = std::char_traits<T>, typename A = std::allocator<T>>
            using std_basic_string = std::basic_string<T, C, A>;
            template <typename K, typename V, typename C = std::less<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_map = std::map<K, V, C, A>;
            template <typename K, typename V, typename C = std::less<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_multimap = std::multimap<K, V, C, A>;
            template <typename K, typename C = std::less<K>, typename A = std::allocator<K>>
            using std_set = std::set<K, C, A>;
            template <typename K, typename C = std::less<K>, typename A = std::allocator<K>>
            using std_multiset = std::multiset<K, C, A>;
            template <typename K, typename V, typename H = std::hash<K>,
                      typename C = std::equal_to<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_unordered_map = std::unordered_map<K, V, H, C, A>;
            template <typename K, typename V, typename H = std::hash<K>,
                      typename C = std::equal_to<K>,
                      typename A = std::allocator<std::pair<K const, V>>>
            using std_unordered_multimap = std::unordered_multimap<K, V, H, C, A>;
            template <typename K, typename H = std::hash<K>, typename C = std::equal_to<K>,
                      typename A = std::allocator<K>>
            using std_unordered_set = std::unordered_set<K, H, C, A>;
            template <typename K, typename H = std::hash<K>, typename C = std::equal_to<K>,
                      typename A = std::allocator<K>>
            using std_unordered_multiset = std::unordered_multiset<K, H, C, A>;
            template <typename T, typename C = std_deque<T>>
            using std_queue = std::queue<T, C>;
            template <typename T, typename C = std_vector<T>,
                      class D = std::less<typename C::value_type>>
            using std_priority_queue = std::priority_queue<T, C, D>;
            template <typename T, typename C = std_deque<T>>
            using std_stack = std::stack<T, C>;
        }

        template <>
        struct quote<::std::list> : quote<detail::std_list>
        {
        };
        template <>
        struct quote<::std::deque> : quote<detail::std_deque>
        {
        };
        template <>
        struct quote<::std::forward_list> : quote<detail::std_forward_list>
        {
        };
        template <>
        struct quote<::std::vector> : quote<detail::std_vector>
        {
        };
        template <>
        struct quote<::std::basic_string> : quote<detail::std_basic_string>
        {
        };
        template <>
        struct quote<::std::map> : quote<detail::std_map>
        {
        };
        template <>
        struct quote<::std::multimap> : quote<detail::std_multimap>
        {
        };
        template <>
        struct quote<::std::set> : quote<detail::std_set>
        {
        };
        template <>
        struct quote<::std::multiset> : quote<detail::std_multiset>
        {
        };
        template <>
        struct quote<::std::unordered_map> : quote<detail::std_unordered_map>
        {
        };
        template <>
        struct quote<::std::unordered_multimap> : quote<detail::std_unordered_multimap>
        {
        };
        template <>
        struct quote<::std::unordered_set> : quote<detail::std_unordered_set>
        {
        };
        template <>
        struct quote<::std::unordered_multiset> : quote<detail::std_unordered_multiset>
        {
        };
        template <>
        struct quote<::std::queue> : quote<detail::std_queue>
        {
        };
        template <>
        struct quote<::std::priority_queue> : quote<detail::std_priority_queue>
        {
        };
        template <>
        struct quote<::std::stack> : quote<detail::std_stack>
        {
        };
    }
}

#endif
/// \endcond

#if defined(__clang__)
#pragma GCC diagnostic pop
#endif
#endif
