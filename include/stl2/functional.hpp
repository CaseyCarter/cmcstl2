// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_FUNCTIONAL_HPP
#define STL2_FUNCTIONAL_HPP

#include <functional>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // <functional> imports
  //
  using std::result_of_t;

  using std::reference_wrapper;
  using std::ref;
  using std::cref;

  using std::plus;
  using std::minus;
  using std::multiplies;
  using std::divides;
  using std::modulus;
  using std::negate;

  using std::logical_and;
  using std::logical_or;
  using std::logical_not;

  using std::bit_and;
  using std::bit_or;
  using std::bit_xor;
  using std::bit_not;

  using std::unary_negate;
  using std::binary_negate;
  using std::not1;
  using std::not2;

  using std::is_bind_expression;
  using std::is_placeholder;
  using std::bind;
  namespace placeholders = std::placeholders;

  using std::mem_fn;
  using std::bad_function_call;

  using std::function;
  using std::hash;

  template <class T>
  struct __unwrap_ : __unwrap_<decay_t<T>> {};
  template <_Decayed T>
  struct __unwrap_<T> { using type = T; };
  template <class T>
  struct __unwrap_<reference_wrapper<T>> { using type = T&; };
  template <class T>
  using __unwrap = meta::_t<__unwrap_<T>>;

  ///////////////////////////////////////////////////////////////////////////
  // identity [func.identity]
  //
  struct identity {
    template <class T>
    constexpr T&& operator()(T&& t) const noexcept {
      return __stl2::forward<T>(t);
    }
  };

  ///////////////////////////////////////////////////////////////////////////
  // invoke [C++ WP]
  //
  namespace __invoke {
    template <_Is<is_function> F, class O, class T, class...Args>
      requires DerivedFrom<decay_t<T>, decay_t<O>>() &&
        requires (F (O::* pmf), T&& t, Args&&...args) {
          (((T&&)t).*pmf)((Args&&)args...);
        }
    constexpr decltype(auto) impl(F (O::*pmf), T&& t, Args&&...args)
    STL2_NOEXCEPT_RETURN(
      (__stl2::forward<T>(t).*pmf)(__stl2::forward<Args>(args)...)
    )

    template <_Is<is_function> F, class O, class T, class...Args>
      requires !DerivedFrom<decay_t<T>, decay_t<O>>() &&
        requires (F (O::*pmf), T&& t, Args&&...args) {
          ((*(T&&)t).*pmf)((Args&&)args...);
        }
    constexpr decltype(auto) impl(F (O::*pmf), T&& t, Args&&...args)
    STL2_NOEXCEPT_RETURN(
      ((*__stl2::forward<T>(t)).*pmf)(__stl2::forward<Args>(args)...)
    )

    template <_IsNot<is_function> M, class O, class T>
      requires DerivedFrom<decay_t<T>, decay_t<O>>() &&
        requires (M (O::*pmd), T&& t) {
          ((T&&)t).*pmd;
        }
    constexpr auto&& impl(M (O::*pmd), T&& t)
    STL2_NOEXCEPT_RETURN(
      __stl2::forward<T>(t).*pmd
    )

    template <_IsNot<is_function> M, class O, class T>
      requires !DerivedFrom<decay_t<T>, decay_t<O>>() &&
        requires (M (O::*pmd), T&& t) {
          (*((T&&)t)).*pmd;
        }
    constexpr auto&& impl(M (O::*pmd), T&& t)
    STL2_NOEXCEPT_RETURN(
      (*__stl2::forward<T>(t)).*pmd
    )

    template <class F, class...Args>
      requires requires (F&& f, Args&&...args) {
        ((F&&)f)((Args&&)args...);
      }
    constexpr decltype(auto) impl(F&& f, Args&&...args)
    STL2_NOEXCEPT_RETURN(
      __stl2::forward<F>(f)(__stl2::forward<Args>(args)...)
    )
  }
  template <class F, class...Args>
    requires requires (F&& f, Args&&...args) {
      __invoke::impl((F&&)f, (Args&&)args...);
    }
  STL2_CONSTEXPR_EXT decltype(auto) invoke(F&& f, Args&&...args)
  STL2_NOEXCEPT_RETURN(
    __invoke::impl(__stl2::forward<F>(f), __stl2::forward<Args>(args)...)
  )

  ///////////////////////////////////////////////////////////////////////////
  // equal_to [comparisons]
  //
  template <class T = void>
    requires Same<T, void>() || EqualityComparable<T>()
  struct equal_to {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a == b);
    }
  };

  template <>
  struct equal_to<void> {
    EqualityComparable{T, U}
    constexpr auto operator()(const T& t, const U& u) const ->
      decltype(t == u) {
      return {t == u};
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // not_equal_to
  //
  template <class T = void>
    requires Same<T, void>() || EqualityComparable<T>()
  struct not_equal_to {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a != b);
    }
  };

  template <>
  struct not_equal_to<void> {
    EqualityComparable{T, U}
    constexpr auto operator()(const T& t, const U& u) const ->
      decltype(t != u) {
      return {t != u};
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // greater
  //
  template <class T = void>
    requires Same<T, void>() || TotallyOrdered<T>()
  struct greater {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a > b);
    }
  };

  template <>
  struct greater<void> {
    TotallyOrdered{T, U}
    constexpr auto operator()(const T& t, const U& u) const ->
      decltype(t > u) {
      return {t > u};
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // less
  //
  template <class T = void>
    requires Same<T, void>() || TotallyOrdered<T>()
  struct less {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a < b);
    }
  };

  template <>
  struct less<void> {
    TotallyOrdered{T, U}
    constexpr auto operator()(const T& t, const U& u) const ->
      decltype(t < u) {
      return {t < u};
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // greater_equal
  //
  template <class T = void>
    requires Same<T, void>() || TotallyOrdered<T>()
  struct greater_equal {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a >= b);
    }
  };

  template <>
  struct greater_equal<void> {
    TotallyOrdered{T, U}
    constexpr auto operator()(const T& t, const U& u) const ->
      decltype(t >= u) {
      return {t >= u};
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // less_equal
  //
  template <class T = void>
    requires Same<T, void>() || TotallyOrdered<T>()
  struct less_equal {
    constexpr bool operator()(const T& a, const T& b) const {
      return bool(a <= b);
    }
  };

  template <>
  struct less_equal<void> {
    TotallyOrdered{T, U}
    constexpr auto operator()(const T& t, const U& u) const ->
      decltype(t <= u) {
      return {t <= u};
    }

    using is_transparent = true_type;
  };

  ///////////////////////////////////////////////////////////////////////////
  // not_fn from N4336 Library Fundamentals v2
  //
  template <class F>
  class __not_fn {
    F f_;

  public:
    template <class FF>
      requires Constructible<F, FF>()
    constexpr __not_fn(FF&& arg)
      noexcept(is_nothrow_constructible<F, FF>::value) :
      f_(__stl2::forward<decltype(arg)>(arg)) {}

    template <class...Args>
      requires requires (F& f, Args&&...args) {
        !__invoke::impl(f, __stl2::forward<Args>(args)...);
      }
    constexpr bool operator()(Args&&...args) &
    STL2_NOEXCEPT_RETURN(
      !__invoke::impl(f_, __stl2::forward<Args>(args)...)
    )

    template <class...Args>
      requires requires (const F& f, Args&&...args) {
        !__invoke::impl(f, __stl2::forward<Args>(args)...);
      }
    constexpr bool operator()(Args&&...args) const&
    STL2_NOEXCEPT_RETURN(
      !__invoke::impl(f_, __stl2::forward<Args>(args)...)
    )

    template <class...Args>
      requires requires (F&& f, Args&&...args) {
        !__invoke::impl((F&&)f, __stl2::forward<Args>(args)...);
      }
    constexpr bool operator()(Args&&...args) &&
    STL2_NOEXCEPT_RETURN(
      !__invoke::impl(__stl2::move(f_), __stl2::forward<Args>(args)...)
    )
  };

  template <class F>
    requires MoveConstructible<F>() &&
      Constructible<decay_t<F>, F>()
  STL2_CONSTEXPR_EXT __not_fn<decay_t<F>> not_fn(F&& f)
    noexcept(is_nothrow_constructible<__not_fn<decay_t<F>>, F>::value) {
    return {__stl2::forward<F>(f)};
  }
} STL2_CLOSE_NAMESPACE

#endif
