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
#ifndef STL2_DETAIL_RANGE_RANGE_HPP
#define STL2_DETAIL_RANGE_RANGE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/compressed_pair.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

STL2_OPEN_NAMESPACE {
  namespace ext {
    // iterator_range? view? simple_iterator_and_sentinel_pair?
    template <Iterator I, Sentinel<I> S>
    class range : ext::compressed_pair<I, S> {
      using pair_t = ext::compressed_pair<I, S>;
    public:
      range() = default;

      constexpr range(I i, S s)
        noexcept(is_nothrow_move_constructible<I>::value &&
                 is_nothrow_move_constructible<S>::value) :
        pair_t{__stl2::move(i), __stl2::move(s)} {}

      constexpr auto begin() const
        noexcept(is_nothrow_copy_constructible<I>::value) {
        return pair_t::first();
      }
      constexpr auto end() const
        noexcept(is_nothrow_copy_constructible<S>::value) {
        return pair_t::second();
      }
    };

    template <Iterator I, Sentinel<I> S>
    constexpr auto make_range(I i, S s)
    STL2_NOEXCEPT_RETURN(
      range<I, S>{__stl2::move(i), __stl2::move(s)}
    )

    template <Iterator I, Sentinel<I> S, class C = CommonType<I, S>>
    constexpr auto make_bounded_range(I i, S s)
    STL2_NOEXCEPT_RETURN(
      range<C, C>{C{__stl2::move(i)}, C{__stl2::move(s)}}
    )

    template <Iterator I>
    constexpr auto make_bounded_range(I i, I s)
    STL2_NOEXCEPT_RETURN(
      range<I, I>{__stl2::move(i), __stl2::move(s)}
    )

    template <RandomAccessIterator I, Sentinel<I> S>
      requires SizedIteratorRange<I, S>()
    constexpr auto make_bounded_range(I i, S s) {
      I end = __stl2::next(i, __stl2::move(s));
      return range<I, I>{__stl2::move(i), __stl2::move(end)};
    }
  }
} STL2_CLOSE_NAMESPACE

#endif
