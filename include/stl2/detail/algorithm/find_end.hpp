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

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// find_end [alg.find.end]
//
STL2_OPEN_NAMESPACE {
	struct __find_end_fn : private __niebloid {
		template<forward_iterator I1, sentinel_for<I1> S1, forward_iterator I2,
			sentinel_for<I2> S2, class Pred = equal_to, class Proj1 = identity,
			class Proj2 = identity>
		requires indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
		constexpr I1 operator()(I1 first1, S1 last1, I2 first2, S2 last2,
			Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			if (first2 == last2) {
				return next(first1, last1);
			}

			if constexpr (bidirectional_iterator<I1> && bidirectional_iterator<I2>) {
				auto end1 = next(first1, last1);
				auto end2 = next(first2, last2);

				if constexpr (random_access_iterator<I1> && random_access_iterator<I2>) {
					// Take advantage of knowing source and pattern lengths.
					// Stop short when source is smaller than pattern
					const auto len2 = end2 - first2;
					if (end1 - first1 < len2) {
						return end1;
					}

					// End of pattern match can't go before here
					const auto s = first1 + (len2 - 1);

					for (auto l1 = end1; l1 != s; --l1) {
						auto m1 = l1;
						auto m2 = end2;
						while (__stl2::invoke(pred,
								__stl2::invoke(proj1, *--m1),
								__stl2::invoke(proj2, *--m2))) {
							if (m2 == first2) {
								return m1;
							}
						}
					}
					return end1;
				} else {
					--end2;
					auto l1 = end1;
					while (l1 != first1) {
						if (__stl2::invoke(pred,
								__stl2::invoke(proj1, *--l1),
								__stl2::invoke(proj2, *end2))) {
							auto m1 = l1;
							auto m2 = end2;
							do {
								if (m2 == first2) {
									return m1;
								}
								if (m1 == first1) {
									return end1;
								}
							} while (__stl2::invoke(pred,
										__stl2::invoke(proj1, *--m1),
										__stl2::invoke(proj2, *--m2)));
						}
					}

					return end1;
				}
			} else {
				std::optional<I1> res;
				for (; first1 != last1; ++first1) {
					if (__stl2::invoke(pred,
							__stl2::invoke(proj1, *first1),
							__stl2::invoke(proj2, *first2))) {
						auto m1 = first1;
						auto m2 = first2;
						do {
							if (++m2 == last2) {
								res = first1;
								break;
							}
							if (++m1 == last1) {
								return std::move(res).value_or(std::move(m1));
							}
						} while (__stl2::invoke(pred,
									__stl2::invoke(proj1, *m1),
									__stl2::invoke(proj2, *m2)));
					}
				}
				return std::move(res).value_or(std::move(first1));
			}
		}


		template<forward_range R1, forward_range R2,
			class Pred = equal_to, class Proj1 = identity, class Proj2 = identity>
		requires indirectly_comparable<iterator_t<R1>, iterator_t<R2>, Pred, Proj1, Proj2>
		constexpr safe_iterator_t<R1> operator()(R1&& r1, R2&& r2,
			Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
		{
			return (*this)(begin(r1), end(r1), begin(r2), end(r2),
				__stl2::ref(pred), __stl2::ref(proj1), __stl2::ref(proj2));
		}
	};

	inline constexpr __find_end_fn find_end{};
} STL2_CLOSE_NAMESPACE

#endif
