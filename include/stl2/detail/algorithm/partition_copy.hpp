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
#ifndef STL2_DETAIL_ALGORITHM_PARTITION_COPY_HPP
#define STL2_DETAIL_ALGORITHM_PARTITION_COPY_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// partition_copy [alg.partitions]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O1, class O2>
	using partition_copy_result = __in_out_out_result<I, O1, O2>;

	struct __partition_copy_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, weakly_incrementable O1,
			weakly_incrementable O2, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		requires indirectly_copyable<I, O1> && indirectly_copyable<I, O2>
		constexpr partition_copy_result<I, O1, O2>
		operator()(I first, S last, O1 out_true, O2 out_false, Pred pred,
			Proj proj = {}) const
		{
			for (; first != last; ++first) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(pred, __stl2::invoke(proj, v))) {
					*out_true  = std::forward<iter_reference_t<I>>(v);
					++out_true;
				} else {
					*out_false = std::forward<iter_reference_t<I>>(v);
					++out_false;
				}
			}
			return {std::move(first), std::move(out_true), std::move(out_false)};
		}

		template<input_range R, weakly_incrementable O1, weakly_incrementable O2,
			class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		requires indirectly_copyable<iterator_t<R>, O1> &&
			indirectly_copyable<iterator_t<R>, O2>
		constexpr partition_copy_result<safe_iterator_t<R>, O1, O2>
		operator()(R&& r, O1 out_true, O2 out_false, Pred pred,
			Proj proj = {}) const
		{
			return (*this)(begin(r), end(r),
				std::move(out_true), std::move(out_false),
				__stl2::ref(pred), __stl2::ref(proj));
		}
	};

	inline constexpr __partition_copy_fn partition_copy {};
} STL2_CLOSE_NAMESPACE

#endif
