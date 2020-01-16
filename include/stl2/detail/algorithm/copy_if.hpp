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
#ifndef STL2_DETAIL_ALGORITHM_COPY_IF_HPP
#define STL2_DETAIL_ALGORITHM_COPY_IF_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_if [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using copy_if_result = __in_out_result<I, O>;

	struct __copy_if_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, weakly_incrementable O,
			class Proj = identity, indirect_unary_predicate<projected<I, Proj>> Pred>
		requires indirectly_copyable<I, O>
		constexpr copy_if_result<I, O>
		operator()(I first, S last, O result, Pred pred, Proj proj = {}) const {
			for (; first != last; ++first) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(pred, __stl2::invoke(proj, v))) {
					*result = static_cast<iter_reference_t<I>&&>(v);
					++result;
				}
			}

			return {static_cast<I&&>(first), static_cast<O&&>(result)};
		}

		template<input_range R, weakly_incrementable O, class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		requires indirectly_copyable<iterator_t<R>, O>
		constexpr copy_if_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, Pred pred, Proj proj = {}) const {
			return (*this)(begin(r), end(r), static_cast<O&&>(result),
				static_cast<Pred&&>(pred), static_cast<Proj&&>(proj));
		}
	};

	inline constexpr __copy_if_fn copy_if{};
} STL2_CLOSE_NAMESPACE

#endif
