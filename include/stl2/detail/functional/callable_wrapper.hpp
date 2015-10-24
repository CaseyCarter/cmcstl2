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
#ifndef STL2_DETAIL_FUNCTIONAL_CALLABLE_WRAPPER_HPP
#define STL2_DETAIL_FUNCTIONAL_CALLABLE_WRAPPER_HPP

#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // callable_wrapper [Extension]
  //
  namespace ext {
    MoveConstructible{F}
    class callable_wrapper : detail::ebo_box<F> {
      using box_t = detail::ebo_box<F>;
    public:
      callable_wrapper() requires DefaultConstructible<F>() = default;

      constexpr callable_wrapper(F&& f)
        noexcept(is_nothrow_constructible<box_t, F&&>::value) :
        box_t{__stl2::move(f)} {}

      constexpr callable_wrapper(const F& f)
        noexcept(is_nothrow_constructible<box_t, const F&>::value)
        requires CopyConstructible<F>() :
        box_t{f} {}

      template <class...Args>
        requires Invokable<F&, Args...>()
      constexpr decltype(auto) operator()(Args&&...args) &
        noexcept(noexcept(
          __invoke::impl(declval<F&>(), __stl2::forward<Args>(args)...)))
      {
        return __invoke::impl(box_t::get(), __stl2::forward<Args>(args)...);
      }

      template <class...Args>
        requires Invokable<const F&, Args...>()
      constexpr decltype(auto) operator()(Args&&...args) const&
        noexcept(noexcept(
          __invoke::impl(declval<const F&>(), __stl2::forward<Args>(args)...)))
      {
        return __invoke::impl(box_t::get(), __stl2::forward<Args>(args)...);
      }

      template <class...Args>
        requires Invokable<F&&, Args...>()
      constexpr decltype(auto) operator()(Args&&...args) &&
        noexcept(noexcept(
          __invoke::impl(declval<F&&>(), __stl2::forward<Args>(args)...)))
      {
        return __invoke::impl(__stl2::move(box_t::get()),
                              __stl2::forward<Args>(args)...);
      }

      template <class...Args>
        requires Invokable<const F&&, Args...>()
      constexpr decltype(auto) operator()(Args&&...args) const&&
        noexcept(noexcept(
          __invoke::impl(declval<const F&&>(), __stl2::forward<Args>(args)...)))
      {
        return __invoke::impl(__stl2::move(box_t::get()),
                              __stl2::forward<Args>(args)...);
      }

      F& base() & { return box_t::get(); }
      const F& base() const& { return box_t::get(); }
      F&& base() && { return __stl2::move(box_t::get()); }
      const F&& base() const&& { return __stl2::move(box_t::get()); }
    };

    template <class F>
      requires Constructible<decay_t<F>, F&&>()
    auto make_callable_wrapper(F&& f) {
      return callable_wrapper<decay_t<F>>{__stl2::forward<F>(f)};
    }

    auto make_callable_wrapper(callable_wrapper<auto>&& f) {
      return __stl2::move(f);
    }

    CopyConstructible{F}
    auto make_callable_wrapper(const callable_wrapper<F>& f) {
      return f;
    }

    auto make_callable_wrapper(std::reference_wrapper<auto> r) {
      return r;
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
