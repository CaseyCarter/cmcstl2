// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015, 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_IS_PERMUTATION_HPP
#define STL2_DETAIL_ALGORITHM_IS_PERMUTATION_HPP

#include <limits>

#include <stl2/detail/algorithm/count_if.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/algorithm/mismatch.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/unreachable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// is_permutation [alg.is_permutation]
//
STL2_OPEN_NAMESPACE {
	struct __is_permutation_fn : private __niebloid {
		template<forward_iterator I1, sentinel_for<I1> S1, forward_iterator I2,
			sentinel_for<I2> S2, class Pred = equal_to, class Proj1 = identity,
			class Proj2 = identity>
		requires indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
		constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2,
			Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			if constexpr (sized_sentinel_for<S1, I1> || sized_sentinel_for<S2, I2>) {
				iter_difference_t<I1> count1;
				if constexpr (sized_sentinel_for<S1, I1>) {
					count1 = last1 - first1;
					if (!__has_length(first2, last2, count1)) return false;
				} else {
					const auto count2 = last2 - first2;
					if (!__has_length(first1, last1, count2)) return false;
					count1 = static_cast<decltype(count1)>(count2);
				}
				return __is_permutation_trim(std::move(first1), std::move(first2),
					count1, pred, proj1, proj2);
			} else {
				// shorten sequences by removing equal prefixes
				auto [mid1, mid2] = mismatch(
					std::move(first1), last1, std::move(first2), last2,
					__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));

				auto [same_length, count] =
					__common_range_length(mid1, std::move(last1), mid2, std::move(last2));
				if (!same_length) return false;

				return __is_permutation_tail(std::move(mid1), std::move(mid2),
					count, pred, proj1, proj2);
			}
		}

		template<forward_range R1, forward_range R2, class Pred = equal_to,
			class Proj1 = identity, class Proj2 = identity>
		requires indirectly_comparable<iterator_t<R1>, iterator_t<R2>, Pred,
			Proj1, Proj2>
		constexpr bool operator()(R1&& r1, R2&& r2, Pred pred = {},
			Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			if constexpr (sized_range<R1> || sized_range<R2>) {
				using D = iter_difference_t<iterator_t<R1>>;
				D count1;
				if constexpr (sized_range<R1>) {
					count1 = distance(r1);
					if (!__has_length(r2, count1)) return false;
				} else {
					const auto count2 = distance(r2);
					if (!__has_length(r1, count2)) return false;
					count1 = static_cast<D>(count2);
				}
				return __is_permutation_trim(begin(r1), begin(r2),
					count1, pred, proj1, proj2);
			} else {
				return (*this)(begin(r1), end(r1), begin(r2), end(r2),
					__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));
			}
		}
	private:
		template<integral To, integral From>
		static constexpr bool __can_represent(const From value) noexcept {
			using C = decltype(true ? value : To{});
			if constexpr (same_as<To, C> && (signed_integral<To> || unsigned_integral<From>)) {
				return true;
			} else {
				if constexpr (signed_integral<From>) {
					if constexpr (unsigned_integral<To>) {
						if (value < 0) return false;
					} else {
						if (C(value) < C(std::numeric_limits<To>::min())) return false;
					}
				}
				return C(value) <= C(std::numeric_limits<To>::max());
			}
		}

		// Does distance(first, last) == n?
		template<input_or_output_iterator I, sentinel_for<I> S, signed_integral D>
		static constexpr bool __has_length(const I first, const S last, const D n) {
			STL2_EXPECT(n >= 0);
			if constexpr (sized_sentinel_for<S, I>) {
				return n == last - first;
			} else {
				if (__can_represent<iter_difference_t<I>>(n)) {
					return advance(first, n, last) == 0 &&
						first == last;
				}
				return false;
			}
		}
		// Does distance(rng) == n?
		template<range Rng, signed_integral D>
		static constexpr bool __has_length(Rng&& rng, const D n) {
			STL2_EXPECT(n >= 0);
			if constexpr (sized_range<Rng>) {
				return n == distance(rng);
			} else {
				return __has_length(begin(rng), end(rng), n);
			}
		}

		// Do the two ranges have the same length, and if so what is it?
		template<input_or_output_iterator I1, sentinel_for<I1> S1,
			input_or_output_iterator I2, sentinel_for<I2> S2>
		static constexpr std::pair<bool, iter_difference_t<I1>>
		__common_range_length(I1 first1, S1 last1, I2 first2, S2 last2) {
			using D = iter_difference_t<I1>;

			if constexpr (sized_sentinel_for<S1, I1>) {
				const auto count = last1 - first1;
				return {__has_length(first2, last2, count), count};
			} else if constexpr (sized_sentinel_for<S2, I2>) {
				const auto count = last2 - first2;
				return {__has_length(first1, last1, count), static_cast<D>(count)};
			} else {
				for (D count = 0;; ++first1, (void)++count, ++first2) {
					const bool end_of_second = first2 == last2;
					if (first1 == last1) return {end_of_second, count};
					if (end_of_second) return {false, count};
				}
			}
		}
		template<range Rng1, range Rng2>
		static constexpr std::pair<bool, iter_difference_t<iterator_t<Rng1>>>
		__common_range_length(Rng1&& rng1, Rng2&& rng2) {
			using D = iter_difference_t<iterator_t<Rng1>>;

			if constexpr (sized_range<Rng1>) {
				auto count = distance(rng1);
				return {__has_length(rng2, count), count};
			} else if constexpr (sized_range<Rng2>) {
				auto count = distance(rng2);
				return {__has_length(rng1, count), static_cast<D>(count)};
			} else {
				return __common_range_length(
					begin(rng1), end(rng1),
					begin(rng2), end(rng2));
			}
		}

		template<forward_iterator I1, forward_iterator I2,
			class Pred, class Proj1, class Proj2>
		requires indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
		static bool __is_permutation_tail(const I1 first1, const I2 first2,
			const iter_difference_t<I1> n, Pred& pred, Proj1& proj1, Proj2& proj2)
		{
			// Pre: [first1, n) and [first2, n) are valid ranges.
			// Pre: n == 0 or *first1 != *first2.
			STL2_EXPECT(n >= 0);
			if (n == 0) return true;
			STL2_ASSERT(!__stl2::invoke(pred, __stl2::invoke(proj1, *first1), __stl2::invoke(proj2, *first2)));
			if (n == 1) return false;

			// For each element in [first1, n), see if there are the same number of
			// equal elements in [first2, n)
			counted_iterator<I1> i{first1, n};
			while (i.count()) {
				auto&& e = __stl2::invoke(proj1, *i);
				auto match_predicate = [&pred, &e](auto&& x) {
					return __stl2::invoke(pred, e, static_cast<decltype(x)&&>(x));
				};

				// If for some j in [first1, i.base()), *j == e, we've already
				// validated the counts of elements equal to e.
				auto match = find_if(counted_iterator{first1, n - i.count()},
					default_sentinel, match_predicate, __stl2::ref(proj1));
				++i;
				if (match.count()) continue;

				// Count number of e in [first2, n)
				const auto c2 = count_if(counted_iterator{first2, n},
					default_sentinel, match_predicate, __stl2::ref(proj2));
				if (c2 == 0) return false;

				// Count number of e in [i, default_sentinel)
				const auto c1 = count_if(i, default_sentinel,
					match_predicate, __stl2::ref(proj1));

				// If the number of e in [first2, n) is not equal to
				// the number of e in [first1, n), we don't have a permutation.
				if (c1 + 1 != c2) return false;
			}
			return true;
		}

		template<forward_iterator I1, forward_iterator I2,
			class Pred, class Proj1, class Proj2>
		requires indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
		static bool __is_permutation_trim(I1 first1, I2 first2, iter_difference_t<I1> n,
			Pred& pred, Proj1& proj1, Proj2& proj2)
		{
			// trim equal prefixes
			auto [counted, mid2] = mismatch(
				counted_iterator{std::move(first1), n}, default_sentinel,
				std::move(first2), unreachable_sentinel,
				__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));

			// TODO: trim equal suffixes from bidirectional sequences?

			return __is_permutation_tail(counted.base(), std::move(mid2),
				counted.count(), pred, proj1, proj2);
		}
	};

	inline constexpr __is_permutation_fn is_permutation{};
} STL2_CLOSE_NAMESPACE

#endif
