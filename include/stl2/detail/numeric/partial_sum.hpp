// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_PARTIAL_SUM_HPP
#define STL2_DETAIL_NUMERIC_PARTIAL_SUM_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/numeric.hpp>
#include <stl2/detail/functional/arithmetic/plus.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class I, class O>
		struct partial_sum_result : __in_out_result<I, O> {};

		struct __partial_sum_fn : private __niebloid {
			template<InputIterator I, Sentinel<I> S1, WeaklyIncrementable O, Sentinel<O> S2,
				class Proj = identity,
				IndirectMagma<projected<I, Proj>, projected<I, Proj>, O> BOp = __stl2::ext::plus>
			requires Constructible<iter_value_t<I>, iter_reference_t<I>>
			constexpr partial_sum_result<I, O>
			operator()(I first, S1 last, O result, S2 result_last, BOp bop = {}, Proj proj = {}) const
			{
				if (first == last || result == result_last) {
					return {std::move(first), std::move(result)};
				}

				auto acc = __stl2::invoke(proj, *first);
				*result++ = acc;
				while (++first != last && result != result_last) {
					acc = __stl2::invoke(bop, std::move(acc), __stl2::invoke(proj, *first));
					*result++ = acc;
				}

				return {std::move(first), std::move(result)};
			}

			template<InputRange R, Range O, class Proj = identity,
				IndirectMagma<projected<iterator_t<R>, Proj>,
				              projected<iterator_t<R>, Proj>,
				              iterator_t<O>> BOp = __stl2::ext::plus>
			requires Constructible<range_value_t<R>, range_reference_t<R>>
			constexpr partial_sum_result<safe_iterator_t<R>, safe_iterator_t<O>>
			operator()(R&& r, O&& result, BOp bop = {}, Proj proj = {}) const {
				return (*this)(
					begin(r), end(r),
					begin(result), end(result),
					std::ref(bop), std::ref(proj)
				);
			}
		};

		inline constexpr __partial_sum_fn partial_sum {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_PARTIAL_SUM_HPP
