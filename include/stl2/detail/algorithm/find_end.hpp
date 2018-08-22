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
#ifndef STL2_DETAIL_ALGORITHM_FIND_END_HPP
#define STL2_DETAIL_ALGORITHM_FIND_END_HPP

#include <optional>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/view/subrange.hpp>

///////////////////////////////////////////////////////////////////////////
// find_end [alg.find.end]
//
STL2_OPEN_NAMESPACE {
	struct __find_end_fn {
		template<ForwardIterator I1, Sentinel<I1> S1, ForwardIterator I2, Sentinel<I2> S2,
			class Pred = ranges::equal_to<>, class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		constexpr subrange<I1> operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			if (first2 == last2) {
				auto end = __stl2::next(first1, last1);
				return {end, end};
			}

			std::optional<I1> res;
			auto const max_distance = __stl2::distance(first2, last2);
	
			for (; first1 != last1; ++first1) {
				if (__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2))) {
					auto m1 = first1;
					auto m2 = first2;
					do {
						if (++m2 == last2) {
							res = first1;
							break;
						}
						if (++m1 == last1) {
							return {std::move(res).value_or(m1), m1};
						}
					} while (__stl2::invoke(pred, __stl2::invoke(proj1, *m1), __stl2::invoke(proj2, *m2)));
				}
			}
			return {std::move(res).value_or(first1), __stl2::next(first1, max_distance, last1)};
		}

		template<BidirectionalIterator I1, BidirectionalIterator I2,
			class Pred = ranges::equal_to<>, class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		constexpr subrange<I1> operator()(I1 first1, I1 last1, I2 first2, I2 last2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			if (first2 == last2) {
				return {last1, last1};  // Everything matches an empty sequence
			}

			--last2;
			auto l1 = last1;
			while (l1 != first1) {
				if (__stl2::invoke(pred, __stl2::invoke(proj1, *--l1), __stl2::invoke(proj2, *last2))) {
					auto m1 = l1;
					auto m2 = last2;
					do {
						if (m2 == first2) {
							return {m1, last1};
						}
						if (m1 == first1) {
							return {last1, last1};
						}
					} while (__stl2::invoke(pred, __stl2::invoke(proj1, *--m1), __stl2::invoke(proj2, *--m2)));
				}
			}

			return {last1, last1};
		}

		template<RandomAccessIterator I1, RandomAccessIterator I2,
			class Pred = ranges::equal_to<>, class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		constexpr subrange<I1> operator()(I1 first1, I1 last1, I2 first2, I2 last2, Pred pred = Pred{},
		Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			// Take advantage of knowing source and pattern lengths.
			// Stop short when source is smaller than pattern
			const auto len2 = last2 - first2;
			if (len2 == 0 || last1 - first1 < len2) {
				return {last1, last1};
			}

			// End of pattern match can't go before here
			const auto s = first1 + (len2 - 1);

			for (auto l1 = last1; l1 != s; --l1) {
				auto m1 = l1;
				auto m2 = last2;
				while (__stl2::invoke(pred, __stl2::invoke(proj1, *--m1), __stl2::invoke(proj2, *--m2))) {
					if (m2 == first2) {
						return {m1, last1};
					}
				}
			}
			return {last1, last1};
		}

		template<BidirectionalIterator I1, Sentinel<I1> S1, BidirectionalIterator I2, Sentinel<I2> S2,
			class Pred = ranges::equal_to<>, class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		constexpr subrange<I1> operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = Pred{},
			Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			auto bi_last1 = __stl2::next(first1, std::move(last1));
			auto bi_last2 = __stl2::next(first2, std::move(last2));
			return (*this)(
				std::move(first1), std::move(bi_last1),
				std::move(first2), std::move(bi_last2),
				std::ref(pred), std::ref(proj1), std::ref(proj2));
		}

		template<ForwardRange Rng1, ForwardRange Rng2, class Pred = ranges::equal_to<>,
			class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<iterator_t<Rng1>, iterator_t<Rng2>, Pred, Proj1, Proj2>
		constexpr safe_subrange_t<Rng1> operator()(Rng1&& rng1, Rng2&& rng2, Pred pred = Pred{},
			Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{}) const
		{
			return (*this)(
				__stl2::begin(rng1), __stl2::end(rng1),
				__stl2::begin(rng2), __stl2::end(rng2),
				std::ref(pred), std::ref(proj1), std::ref(proj2));
		}

		// Holding off on initializer_list overloads for now; this
		// overload set is already very fragile.
	};

	inline constexpr __find_end_fn find_end {};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_ALGORITHM_FIND_END_HPP
