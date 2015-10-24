// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_FUNCTIONAL_INVOKE_HPP
#define STL2_DETAIL_FUNCTIONAL_INVOKE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
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
  // Invokable [Extension]
  //
  namespace ext {
    template <class F, class...Args>
    constexpr bool __invokable = false;
    template <class F, class...Args>
      requires requires (F&& f, Args&&...args) {
        __invoke::impl((F&&)f, (Args&&)args...);
      }
    constexpr bool __invokable<F, Args...> = true;

    template <class F, class...Args>
    concept bool Invokable() {
      return __invokable<F, Args...>;
    }
  }

  namespace models {
    template <class, class...>
    constexpr bool Invokable = false;
    ext::Invokable{F, ...Args}
    constexpr bool Invokable<F, Args...> = true;
  }
} STL2_CLOSE_NAMESPACE

#endif
