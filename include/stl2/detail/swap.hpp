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
#ifndef STL2_DETAIL_SWAP_HPP
#define STL2_DETAIL_SWAP_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // exchange [utility.exchange]
  //
  template <Movable T, class U = T>
    requires Assignable<T&, U>()
  constexpr T exchange(T& t, U&& u)
    noexcept(is_nothrow_move_constructible<T>::value &&
             is_nothrow_assignable<T&, U>::value) {
    T tmp(__stl2::move(t));
    t = __stl2::forward<U>(u);
    return tmp;
  }

  ///////////////////////////////////////////////////////////////////////////
  // swap [utility.swap]
  //
  namespace __swap {
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2152
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#2171

    // Extension: less specialized than overloads on different Movable types.
    template <Movable T, Movable U>
      requires Same<T, U>()
    constexpr void swap(T& a, U& b)
      noexcept(noexcept(b = __stl2::exchange(a, __stl2::move(b)))) {
      b = __stl2::exchange(a, __stl2::move(b));
    }

    template <class T, class U>
    constexpr bool __lvalue_swappable = false;
    template <class T, class U>
      requires requires (T& x, U& y) { swap(x, y); }
    constexpr bool __lvalue_swappable<T, U> = true;

    template <class C, class T, class U>
    constexpr bool __recursive_swap_test = false;
    template <class C, class T, class U>
      requires requires (T& x, U& y) { C{}(x, y); }
    constexpr bool __recursive_swap_test<C, T, U> = true;

    struct __try_swap_fn {
      template <class T, class U>
        requires __lvalue_swappable<T, U>
      void operator()(T& t, U& u) const
        noexcept(noexcept(swap(t,u)));

      template <class T, class U, std::size_t N, typename This = __try_swap_fn>
        requires __recursive_swap_test<This, T, U>
      void operator()(T (&t)[N], U (&u)[N]) const
        noexcept(noexcept(This{}(*t, *u)));
    };
    // Workaround GCC PR66957 by declaring this unnamed namespace inline.
    inline namespace {
      constexpr auto& __try_swap = detail::static_const<__try_swap_fn>::value;
    }

    // 20150715: Conforming extension: can swap T(&)[N] with U(&)[N]
    // if T& and U& are Swappable.
    template <class T, class U>
    constexpr bool __array_swap_test = false;
    template <class T, class U>
      requires requires (T &x, U &y) { __try_swap(x, y); }
    constexpr bool __array_swap_test<T, U> = true;

    template <class T, class U, std::size_t N>
      requires __array_swap_test<T, U>
    constexpr void swap(T (&t)[N], U (&u)[N])
      noexcept(noexcept(__try_swap(*t, *u)));

    template <class T, class U>
    constexpr bool can_swap = false;
    template <class T, class U>
      requires requires (T&& a, U&& b) { swap((T&&)a, (U&&)b); }
    constexpr bool can_swap<T, U> = true;

    struct fn {
      template <class T, class U>
        requires can_swap<T, U>
      constexpr void operator()(T&& a, U&& b) const
      STL2_NOEXCEPT_RETURN(
        (void)swap(__stl2::forward<T>(a), __stl2::forward<U>(b))
      )
    };
  }
  // Workaround GCC PR66957 by declaring this unnamed namespace inline.
  inline namespace {
    // 20150805: Not to spec: swap is a N4381-style function object customization point.
    constexpr auto& swap = detail::static_const<__swap::fn>::value;
  }

  namespace __swap {
    template <class T, class U, std::size_t N>
      requires __array_swap_test<T, U>
    constexpr void swap(T (&t)[N], U (&u)[N])
      noexcept(noexcept(__try_swap(*t, *u))) {
      for (std::size_t i = 0; i < N; ++i) {
        __stl2::swap(t[i], u[i]);
      }
    }
  }

  ///////////////////////////////////////////////////////////////////////////
  // Swappable [concepts.lib.corelang.swappable]
  //
  template <class T, class U>
  constexpr bool __swappable = false;
  template <class T, class U>
    requires requires (T&& t, U&&u) {
      __stl2::swap((T&&)t, (U&&)u);
    }
  constexpr bool __swappable<T, U> = true;

  template <class T>
  concept bool Swappable() {
    return __swappable<T, T>;
  }

  template <class T, class U>
  concept bool Swappable() {
    return Swappable<T>() &&
      Swappable<U>() &&
      CommonReference<const T&, const U&>() &&
      __swappable<T, U> &&
      __swappable<U, T>;
  }

  namespace models {
    template <class T, class U = T>
    constexpr bool Swappable = false;
    __stl2::Swappable{T}
    constexpr bool Swappable<T, T> = true;
    __stl2::Swappable{T, U}
    constexpr bool Swappable<T, U> = true;

    // Swappable<Ts>() && ...
    template <class...Ts>
    constexpr bool AllSwappable = false;
    template <__stl2::Swappable...Ts>
    constexpr bool AllSwappable<Ts...> = true;
  }

  template <class T, class U>
  constexpr bool is_nothrow_swappable_v = false;

  Swappable{T, U}
  constexpr bool is_nothrow_swappable_v<T, U> =
    noexcept(__stl2::swap(__stl2::declval<T>(), __stl2::declval<U>())) &&
    noexcept(__stl2::swap(__stl2::declval<U>(), __stl2::declval<T>()));

  template <class T, class U>
  using is_nothrow_swappable_t =
    meta::bool_<is_nothrow_swappable_v<T, U>>;

  template <class T, class U>
  struct is_nothrow_swappable :
    is_nothrow_swappable_t<T, U> {};
} STL2_CLOSE_NAMESPACE

#endif
