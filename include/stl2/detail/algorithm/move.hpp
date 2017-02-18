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
		IndirectlyMovable<I, O>()
	tagged_pair<tag::in(I), tag::out(O)>
	move(I first, S last, O result) {
#if 1
		auto res = __stl2::copy(__stl2::make_move_iterator(__stl2::move(first)),
			__stl2::make_move_sentinel(__stl2::move(last)), __stl2::move(result));
		return {__stl2::move(res.in()).base(), __stl2::move(res.out())};
#else
		for (; first != last; ++first, ++result) {
			*result = __stl2::iter_move(first);
		}
		return {__stl2::move(first), __stl2::move(result)};
#endif
	}

	template <InputRange Rng, class O>
	requires
		WeaklyIncrementable<__f<O>>() &&
		IndirectlyMovable<iterator_t<Rng>, __f<O>>()
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	move(Rng&& rng, O&& result) {
		return __stl2::move(__stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result));
	}

	// Extension
	template <class E, class O>
	requires
		WeaklyIncrementable<__f<O>>() &&
		IndirectlyMovable<const E*, __f<O>>()
	tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
	move(std::initializer_list<E>&& rng, O&& result) {
		return __stl2::move(__stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result));
	}

	namespace ext {
		// Extension
		template <InputIterator I1, Sentinel<I1> S1, Iterator I2, Sentinel<I2> S2>
		requires
			IndirectlyMovable<I1, I2>()
		tagged_pair<tag::in(I1), tag::out(I2)>
		move(I1 first1, S1 last1, I2 first2, S2 last2) {
#if 1
			auto res = ext::copy(
				__stl2::make_move_iterator(__stl2::move(first1)),
				__stl2::make_move_sentinel(__stl2::move(last1)),
				__stl2::make_move_iterator(__stl2::move(first2)),
				__stl2::make_move_sentinel(__stl2::move(last2)));
			return {__stl2::move(res.in()).base(), __stl2::move(res.out())};
#else
			for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
				*first2 = __stl2::iter_move(first1);
			}
			return {__stl2::move(first1), __stl2::move(first2)};
#endif
		}

		// Extension
		template <InputRange Rng1, Range Rng2>
		requires
			IndirectlyMovable<iterator_t<Rng1>, iterator_t<Rng2>>()
		tagged_pair<
			tag::in(safe_iterator_t<Rng1>),
			tag::out(safe_iterator_t<Rng2>)>
		move(Rng1&& rng1, Rng2&& rng2) {
			return ext::move(__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), __stl2::end(rng2));
		}

		// Extension
		template <class E, Range Rng2>
		requires
			IndirectlyMovable<const E*, iterator_t<Rng2>>()
		tagged_pair<
			tag::in(dangling<const E*>),
			tag::out(safe_iterator_t<Rng2>)>
		move(std::initializer_list<E>&& rng1, Rng2&& rng2) {
			return ext::move(__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), __stl2::end(rng2));
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
