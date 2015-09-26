// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
// Copyright (c) 2009 Alexander Stepanov and Paul McJones
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. The authors make no
// representations about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.
//
// Algorithms from
// Elements of Programming
// by Alexander Stepanov and Paul McJones
// Addison-Wesley Professional, 2009
//
#ifndef STL2_DETAIL_ALGORITHM_REVERSE_HPP
#define STL2_DETAIL_ALGORITHM_REVERSE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/temporary_vector.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/reverse_move.hpp>
#include <stl2/detail/algorithm/swap_ranges.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse [alg.reverse]
//
STL2_OPEN_NAMESPACE {
  namespace detail {
    // From EoP
    Permutable{I}
    I reverse_n_with_buffer(I first, DifferenceType<I> n,
                            temporary_buffer<ValueType<I>>& buf) {
      // Precondition: $\property{mutable\_counted\_range}(first, n)$
      STL2_ASSERT(n <= buf.size());
      auto&& vec = detail::make_temporary_vector(buf);
      __stl2::move(__stl2::make_counted_iterator(first, n),
                   __stl2::default_sentinel{},
                   __stl2::back_inserter(vec));
      return __stl2::reverse_move(vec.begin(), vec.end(),
                                  __stl2::move(first)).out();
    }

    Permutable{I}
    inline void swap_ranges_n(I first, I middle, DifferenceType<I> n) {
      auto i = __stl2::make_counted_iterator(__stl2::move(first), n);
      __stl2::swap_ranges(__stl2::move(i), __stl2::default_sentinel{},
                          __stl2::move(middle));
    }

    // From EoP
    Permutable{I}
    I reverse_n_adaptive(I first, const DifferenceType<I> n,
                         temporary_buffer<ValueType<I>>& buf) {
      // Precondition: $\property{mutable\_counted\_range}(first, n)$
      if (n < DifferenceType<I>(2)) {
        return __stl2::next(__stl2::move(first), n);
      }
      if (n <= buf.size()) {
        return detail::reverse_n_with_buffer(__stl2::move(first), n, buf);
      }
      auto half_n = n / 2;
      auto middle = detail::reverse_n_adaptive(first, half_n, buf);
      if (n % 2 != 0) {
        ++middle;
      }
      auto last = detail::reverse_n_adaptive(middle, half_n, buf);
      swap_ranges_n(__stl2::move(first), __stl2::move(middle), half_n);
      return last;
    }

    Permutable{I}
    I reverse_n(I first, DifferenceType<I> n) {
      using buf_t = temporary_buffer<ValueType<I>>;
      buf_t buf{};
      // TODO: tune this threshold.
      if (n >= DifferenceType<I>(16)) {
        buf = buf_t{n};
      }
      return detail::reverse_n_adaptive(__stl2::move(first), n, buf);
    }
  }

  template <BidirectionalIterator I>
    requires Permutable<I>()
  I reverse(I first, I last) {
    auto m = last;
    while (first != m && first != --m) {
      __stl2::iter_swap2(first, m);
      ++first;
    }
    return last;
  }

  template <RandomAccessIterator I>
    requires Permutable<I>()
  I reverse(I first, I last) {
    if (first != last) {
      auto m = last;
      while (first < --m) {
        __stl2::iter_swap2(first, m);
        ++first;
      }
    }
    return last;
  }
  
  // Extension
  template <Permutable I, Sentinel<I> S>
  I reverse(I first, S last) {
    auto n = __stl2::distance(first, __stl2::move(last));
    return detail::reverse_n(__stl2::move(first), n);
  }
  
  template <Permutable I, Sentinel<I> S>
    requires BidirectionalIterator<I>()
  I reverse(I first, S last) {
    auto bound = __stl2::next(first, __stl2::move(last));
    return __stl2::reverse(__stl2::move(first), __stl2::move(bound));
  }

  // Extension
  template <ForwardRange Rng>
    requires Permutable<IteratorType<Rng>>()
  safe_iterator_t<Rng> reverse(Rng&& rng) {
    return detail::reverse_n(__stl2::begin(rng), __stl2::distance(rng));
  }

  template <BidirectionalRange Rng>
    requires Permutable<IteratorType<Rng>>()
  safe_iterator_t<Rng> reverse(Rng&& rng) {
    return __stl2::reverse(__stl2::begin(rng), __stl2::end(rng));
  }
} STL2_CLOSE_NAMESPACE

#endif
