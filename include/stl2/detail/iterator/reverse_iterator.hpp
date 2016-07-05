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
#ifndef STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse_iterator [reverse.iterator]
//
STL2_OPEN_NAMESPACE {
  namespace __reverse_iterator {
    BidirectionalIterator{I}
    class cursor;

    struct access {
      template <_SpecializationOf<cursor> C>
      static constexpr decltype(auto) current(C&& c) noexcept {
        return (__stl2::forward<C>(c).current_);
      }
    };

    BidirectionalIterator{I}
    class cursor {
      friend access;
      I current_{};

    public:
      using difference_type = difference_type_t<I>;
      using value_type = value_type_t<I>;

      class mixin : protected detail::ebo_box<cursor> {
        using base_t = detail::ebo_box<cursor>;
      public:
        using iterator_type = I;
        using difference_type = cursor::difference_type;
        using value_type = cursor::value_type;
        using iterator_category = iterator_category_t<I>;
        using reference = reference_t<I>;
        using pointer = I;

        mixin() = default;
        using base_t::base_t;

        STL2_CONSTEXPR_EXT I base() const
        noexcept(is_nothrow_copy_constructible<I>::value)
        { return base_t::get().current_; }
      };

      cursor() = default;
      STL2_CONSTEXPR_EXT explicit cursor(I x)
      noexcept(is_nothrow_move_constructible<I>::value)
      : current_{__stl2::move(x)}
      {}
      template <ConvertibleTo<I> U>
      STL2_CONSTEXPR_EXT cursor(const cursor<U>& u)
      noexcept(is_nothrow_constructible<I, const U&>::value)
      : current_{access::current(u)}
      {}

      STL2_CONSTEXPR_EXT reference_t<I> read() const
      STL2_NOEXCEPT_RETURN(
        *__stl2::prev(current_)
      )

      STL2_CONSTEXPR_EXT I arrow() const
      STL2_NOEXCEPT_RETURN(
        __stl2::prev(current_)
      )

      STL2_CONSTEXPR_EXT void next()
      STL2_NOEXCEPT_RETURN(
        (void)--current_
      )

      STL2_CONSTEXPR_EXT void prev()
      STL2_NOEXCEPT_RETURN(
        (void)++current_
      )

      STL2_CONSTEXPR_EXT void advance(difference_type_t<I> n)
      noexcept(noexcept(current_ -= n))
      requires RandomAccessIterator<I>()
      {
        current_ -= n;
      }

      STL2_CONSTEXPR_EXT bool equal(
        const cursor<EqualityComparable<I> >& that) const
      STL2_NOEXCEPT_RETURN(
        current_ == access::current(that)
      )

      STL2_CONSTEXPR_EXT difference_type_t<I> distance_to(
        const cursor<SizedSentinel<I> >& that) const
      STL2_NOEXCEPT_RETURN(
        -(access::current(that) - current_)
      )

      // Extension
      STL2_CONSTEXPR_EXT decltype(auto) indirect_move() const
      STL2_NOEXCEPT_RETURN(
        __stl2::iter_move(__stl2::prev(current_))
      )

      // Extension
      STL2_CONSTEXPR_EXT void indirect_swap(
        const cursor<IndirectlySwappable<I> >& that) const
      STL2_NOEXCEPT_RETURN(
        __stl2::iter_swap(
          __stl2::prev(current_), __stl2::prev(access::current(that)))
      )
    };
  }

  BidirectionalIterator{I}
  using reverse_iterator = basic_iterator<__reverse_iterator::cursor<I>>;

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator<(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    __reverse_iterator::access::current(__stl2::get_cursor(x)) >
      __reverse_iterator::access::current(__stl2::get_cursor(y))
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator>(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    y < x
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator<=(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    !(y < x)
  )

  StrictTotallyOrdered{I1, I2}
  STL2_CONSTEXPR_EXT bool operator>=(
    const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
  STL2_NOEXCEPT_RETURN(
    !(x < y)
  )

  template <class I>
  requires BidirectionalIterator<__f<I>>()
  STL2_CONSTEXPR_EXT auto make_reverse_iterator(I&& i)
  STL2_NOEXCEPT_RETURN(
    reverse_iterator<__f<I>>{__stl2::forward<I>(i)}
  )
} STL2_CLOSE_NAMESPACE

#endif
