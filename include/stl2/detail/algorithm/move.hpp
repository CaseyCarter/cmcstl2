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
#ifndef STL2_DETAIL_ALGORITHM_MOVE_HPP
#define STL2_DETAIL_ALGORITHM_MOVE_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/algorithm/copy.hpp>

///////////////////////////////////////////////////////////////////////////
// move [alg.move]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
	requires
		IndirectlyMovable<I, O>
	tagged_pair<tag::in(I), tag::out(O)>
	move(I first, S last, O result) {
		for (; first != last; ++first, ++result) {
			*result = __stl2::iter_move(first);
		}
		return {std::move(first), std::move(result)};
	}

	template <InputRange Rng, class O>
	requires
		WeaklyIncrementable<__f<O>> &&
		IndirectlyMovable<iterator_t<Rng>, __f<O>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	move(Rng&& rng, O&& result) {
		return __stl2::move(__stl2::begin(rng), __stl2::end(rng), std::forward<O>(result));
	}

	namespace ext {
		// Extension
		template <InputIterator I1, Sentinel<I1> S1, Iterator I2, Sentinel<I2> S2>
		requires
			IndirectlyMovable<I1, I2>
		tagged_pair<tag::in(I1), tag::out(I2)>
		move(I1 first1, S1 last1, I2 first2, S2 last2) {
			for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
				*first2 = __stl2::iter_move(first1);
			}
			return {std::move(first1), std::move(first2)};
		}

		// Extension
		template <InputRange Rng1, Range Rng2>
		requires
			IndirectlyMovable<iterator_t<Rng1>, iterator_t<Rng2>>
		tagged_pair<
			tag::in(safe_iterator_t<Rng1>),
			tag::out(safe_iterator_t<Rng2>)>
		move(Rng1&& rng1, Rng2&& rng2) {
			return ext::move(__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), __stl2::end(rng2));
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
