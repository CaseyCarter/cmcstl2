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
#ifndef STL2_DETAIL_ALGORITHM_FILL_HPP
#define STL2_DETAIL_ALGORITHM_FILL_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>

///////////////////////////////////////////////////////////////////////////
// fill [alg.fill]
STL2_OPEN_NAMESPACE {
  template <class T, OutputIterator<T> O, Sentinel<O> S>
  O fill(O first, S last, const T& value)
  {
    for (; first != last; ++first) {
      *first = value;
    }
    return first;
  }

  template <class T, OutputRange<T> Rng>
  safe_iterator_t<Rng> fill(Rng&& rng, const T& value)
  {
    return __stl2::fill(__stl2::begin(rng), __stl2::end(rng), value);
  }
} STL2_CLOSE_NAMESPACE

#endif
