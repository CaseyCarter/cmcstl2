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
#ifndef STL2_DETAIL_VIEW_REPEAT_N_HPP
#define STL2_DETAIL_VIEW_REPEAT_N_HPP

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/view/repeat.hpp>

STL2_OPEN_NAMESPACE {
  template <Semiregular T>
  class repeat_n_view : detail::ebo_box<repeat_view<T>> {
    using repeat_t = detail::ebo_box<repeat_view<T>>;
    std::ptrdiff_t n_;
  public:
    repeat_n_view() = default;
    repeat_n_view(T value, std::ptrdiff_t n)
      noexcept(is_nothrow_constructible<repeat_t, T>::value) :
      repeat_t{__stl2::move(value)}, n_{n} {}

    using iterator = counted_iterator<typename repeat_view<T>::iterator>;

    constexpr iterator begin() const
      noexcept(noexcept(iterator{declval<const repeat_view<T>&>().begin(), 0})) {
      return {repeat_t::get().begin(), n_};
    }
    constexpr default_sentinel end() const noexcept {
      return {};
    }

    constexpr const T& value() const noexcept {
      return repeat_t::get().value();
    }
  };
} STL2_CLOSE_NAMESPACE

#endif
