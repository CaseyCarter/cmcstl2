// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_SHIFT_LEFT_HPP
#define STL2_DETAIL_ALGORITHM_SHIFT_LEFT_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/access.hpp>

///////////////////////////////////////////////////////////////////////////
// shift_left [Extension, from P0769]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		template <ForwardIterator I, Sentinel<I> S>
		requires IndirectlyMovable<I, I>
		I shift_left(I first, S last, const difference_type_t<I> n) {
			if (n <= 0) {
				return __stl2::next(first, last);
			}

			auto mid = first;
			if (__stl2::advance(mid, n, last) > 0) {
				return first;
			}

			return ext::move_overlapping(std::move(mid), std::move(last), std::move(first)).out();
		}

		template <ForwardRange Rng>
		requires IndirectlyMovable<iterator_t<Rng>, iterator_t<Rng>>
		safe_iterator_t<Rng> shift_left(Rng&& rng, const difference_type_t<iterator_t<Rng>> n) {
			return __stl2::shift_left(__stl2::begin(rng), __stl2::end(rng), n);
		}
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
