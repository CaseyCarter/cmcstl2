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
#ifndef STL2_DETAIL_ALGORITHM_SHIFT_RIGHT_HPP
#define STL2_DETAIL_ALGORITHM_SHIFT_RIGHT_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/access.hpp>

///////////////////////////////////////////////////////////////////////////
// shift_right [Extension, from P0769]
//
STL2_OPEN_NAMESPACE {
	namespace ext {
		template <ForwardIterator I, Sentinel<I> S>
		requires IndirectlyMovable<I, I> && IndirectlySwappable<I>
		I shift_right(I first, const S last, const difference_type_t<I> n) {
			if (n <= 0) {
				return first;
			}

			const auto left = first;
			if (__stl2::advance(first, n, last) != 0) {
				return first;
			}
			const auto right = first;
			// [left, right) is the prefix of n elements that are left
			// valid-but-unspecified; we'll use it as scratch space

			auto probe = first;
			if (const auto d = __stl2::advance(probe, n, last)) {
				return ext::move_overlapping(left, __stl2::next(left, n - d), first).out();
			}

			auto mid = left;
			for (;;) {
				__stl2::iter_swap(first, mid);
				if (++mid == right) {
					mid = left;
				}
				++first;
				if (++probe == last) {
					break;
				}
			}

			for (;;) {
				*first = __stl2::iter_move(mid);
				if (++first == last) {
					break;
				}
				if (++mid == right) {
					mid = left;
				}
			}
			return first;
		}

		template <BidirectionalIterator I, Sentinel<I> S>
		requires IndirectlyMovable<I, I>
		I shift_right(I first, const S last_, const difference_type_t<I> n) {
			auto last = __stl2::next(first, last_);
			auto mid = first;
			if (__stl2::advance(mid, n, last) == 0) {
				ext::move_backward_overlapping();
			}
		}

		template <ForwardRange Rng>
		requires IndirectlyMovable<iterator_t<Rng>, iterator_t<Rng>>
		safe_iterator_t<Rng> shift_right(Rng&& rng, const difference_type_t<iterator_t<Rng>> n) {
			return __stl2::shift_right(__stl2::begin(rng), __stl2::end(rng), n);
		}
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
