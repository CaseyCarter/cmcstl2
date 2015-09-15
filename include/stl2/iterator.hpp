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
#ifndef STL2_ITERATOR_HPP
#define STL2_ITERATOR_HPP

#include <initializer_list>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/detail/iterator/istream_iterator.hpp>
#include <stl2/detail/iterator/istreambuf_iterator.hpp>
#include <stl2/detail/iterator/ostreambuf_iterator.hpp>
#include <stl2/detail/iterator/move_iterator.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>

STL2_OPEN_NAMESPACE {
  using std::iterator;

  template <CopyConstructible T>
  class dangling {
  public:
    constexpr dangling()
      noexcept(is_nothrow_default_constructible<T>::value)
      requires DefaultConstructible<T>()
      : value{}
    { }
    constexpr dangling(T t)
      noexcept(is_nothrow_move_constructible<T>::value)
      : value(__stl2::move(t))
    { }
    constexpr T get_unsafe() const&
      noexcept(is_nothrow_copy_constructible<T>::value) {
      return value;
    }
    constexpr T get_unsafe() &&
      noexcept(is_nothrow_move_constructible<T>::value) {
      return __stl2::move(value);
    }
  private:
    T value;
  };

  template <Range R>
  using safe_iterator_t =
    meta::if_<is_lvalue_reference<R>, IteratorType<R>, dangling<IteratorType<R>>>;

  // Extension: unreachable is WeaklyTotallyOrdered with *any* type.
  class unreachable {};

  constexpr bool operator==(const auto&, unreachable) noexcept {
    return false;
  }
  constexpr bool operator==(unreachable, const auto&) noexcept {
    return false;
  }
  constexpr bool operator==(unreachable, unreachable) noexcept {
    return true;
  }

  constexpr bool operator!=(const auto&, unreachable) noexcept {
    return true;
  }
  constexpr bool operator!=(unreachable, const auto&) noexcept {
    return true;
  }
  constexpr bool operator!=(unreachable, unreachable) noexcept {
    return false;
  }

  constexpr bool operator<(const auto&, unreachable) noexcept {
    return true;
  }
  constexpr bool operator<(unreachable, const auto&) noexcept {
    return false;
  }
  constexpr bool operator<(unreachable, unreachable) noexcept {
    return false;
  }

  constexpr bool operator>(const auto&, unreachable) noexcept {
    return false;
  }
  constexpr bool operator>(unreachable, const auto&) noexcept {
    return true;
  }
  constexpr bool operator>(unreachable, unreachable) noexcept {
    return false;
  }

  constexpr bool operator<=(const auto&, unreachable) noexcept {
    return true;
  }
  constexpr bool operator<=(unreachable, const auto&) noexcept {
    return false;
  }
  constexpr bool operator<=(unreachable, unreachable) noexcept {
    return true;
  }

  constexpr bool operator>=(const auto&, unreachable) noexcept {
    return false;
  }
  constexpr bool operator>=(unreachable, const auto&) noexcept {
    return true;
  }
  constexpr bool operator>=(unreachable, unreachable) noexcept {
    return true;
  }

  namespace ext {
    template <Iterator I, Sentinel<I> S>
    class range { // iterator_range? view? simple_iterator_and_sentinel_pair?
    private:
      // Todo: EBO
      I i_;
      S s_;

    public:
      range() = default;

      constexpr range(I i, S s)
        noexcept(is_nothrow_move_constructible<I>::value &&
                 is_nothrow_move_constructible<S>::value) :
        i_{__stl2::move(i)}, s_{__stl2::move(s)} {}

      constexpr auto begin() const
        noexcept(is_nothrow_copy_constructible<I>::value) {
        return i_;
      }
      constexpr auto end() const
        noexcept(is_nothrow_copy_constructible<S>::value) {
        return s_;
      }
    };

    template <Iterator I, Sentinel<I> S = I>
    constexpr auto make_range(I i, S s = S{})
    STL2_NOEXCEPT_RETURN(
      range<I, S>{__stl2::move(i), __stl2::move(s)}
    )

    template <Iterator I, Sentinel<I> S = I, class C = CommonType<I, S>>
    constexpr auto make_bounded_range(I i, S s = S{})
    STL2_NOEXCEPT_RETURN(
      __stl2::ext::make_range(C{__stl2::move(i)}, C{__stl2::move(s)})
    )
  }
} STL2_CLOSE_NAMESPACE

#endif
