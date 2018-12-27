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
#ifndef STL2_DETAIL_ALGORITHM_EQUAL_HPP
#define STL2_DETAIL_ALGORITHM_EQUAL_HPP

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// equal [alg.equal]
//
STL2_OPEN_NAMESPACE {
	struct __equal_fn : private __niebloid {
	private:
		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2,
			class Pred, class Proj1, class Proj2>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		static constexpr bool __equal_3(I1 first1, S1 last1, I2 first2,
			Pred& pred, Proj1& proj1, Proj2& proj2)
		{
			for (; first1 != last1; (void) ++first1, (void) ++first2) {
				if (!__stl2::invoke(pred,
						__stl2::invoke(proj1, *first1),
						__stl2::invoke(proj2, *first2))) {
					return false;
				}
			}
			return true;
		}

		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2,
			Sentinel<I2> S2, class Pred, class Proj1, class Proj2>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		static constexpr bool __equal_4(I1 first1, S1 last1, I2 first2, S2 last2,
			Pred& pred, Proj1& proj1, Proj2& proj2)
		{
			while (true) {
				const bool b = first2 == last2;
				if (first1 == last1) return b;
				if (b) return false;
				if (!__stl2::invoke(pred,
						__stl2::invoke(proj1, *first1),
						__stl2::invoke(proj2, *first2))) return false;
				++first1;
				++first2;
			}
		}
	public:
		template<InputIterator I1, Sentinel<I1> S1, InputIterator I2, Sentinel<I2> S2,
			class Pred = equal_to, class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
		constexpr bool operator()(I1 first1, S1 last1, I2 first2, S2 last2, Pred pred = {},
			Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			if constexpr (SizedSentinel<S1, I1> && SizedSentinel<S2, I2>) {
				auto len1 = distance(first1, last1);
				auto len2 = distance(first2, std::move(last2));
				return len1 == len2 &&
					__equal_3(std::move(first1), std::move(last1),
						std::move(first2), pred, proj1, proj2);
			} else {
				return __equal_4(
					std::move(first1), std::move(last1),
					std::move(first2), std::move(last2),
					pred, proj1, proj2);
			}
		}

		template<InputRange R1, InputRange R2, class Pred = equal_to,
			class Proj1 = identity, class Proj2 = identity>
		requires IndirectlyComparable<iterator_t<R1>, iterator_t<R2>, Pred, Proj1, Proj2>
		constexpr bool operator()(R1&& r1, R2&& r2, Pred pred = {},
			Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			if constexpr (SizedRange<R1> && SizedRange<R2>) {
				return distance(r1) == distance(r2) &&
					__equal_3(
						begin(r1), end(r1),
						begin(r2), pred, proj1, proj2);
			} else {
				return __equal_4(
					begin(r1), end(r1),
					begin(r2), end(r2),
					pred, proj1, proj2);
			}
		}
	};

	inline constexpr __equal_fn equal {};
} STL2_CLOSE_NAMESPACE

#endif
