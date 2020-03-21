// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP
#define STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/numeric.hpp>
#include <stl2/detail/functional/arithmetic/minus.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class I, class O>
		struct adjacent_difference_result : __in_out_result<I, O> {};

		struct __adjacent_difference_fn : private __niebloid {
			template<InputIterator I, Sentinel<I> S1, WeaklyIncrementable O, Sentinel<O> S2,
			         class Proj = identity,
			         IndirectMagma<projected<I, Proj>, projected<I, Proj>, O> BOp = __stl2::ext::minus>
			constexpr adjacent_difference_result<I, O>
			operator()(I first, S1 last, O result, S2 result_last, BOp bop = {}, Proj proj = {}) const
			{
				if (first == last || result == result_last) {
					return {std::move(first), std::move(result)};
				}

				auto acc = __stl2::invoke(proj, *first);
				*result = acc;
				++result;
				while (++first != last && result != result_last) {
					auto current = __stl2::invoke(proj, *first);
					*result = __stl2::invoke(bop, current, std::move(acc));
					++result;
					acc = std::move(current);
				}

				return {std::move(first), std::move(result)};
			}

			template<InputRange R, Range O, class Proj = identity,
			         IndirectMagma<projected<iterator_t<R>, Proj>,
			                       projected<iterator_t<R>, Proj>,
			                       iterator_t<O>> BOp = __stl2::ext::minus>
			constexpr adjacent_difference_result<safe_iterator_t<R>, safe_iterator_t<O>>
			operator()(R&& r, O&& result, BOp bop = {}, Proj proj = {}) const
			{
				return (*this)(begin(r), end(r), begin(result), end(result), std::ref(bop),
					std::ref(proj));
			}
		};

		inline constexpr __adjacent_difference_fn adjacent_difference {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_ADJACENT_DIFFERENCE_HPP
