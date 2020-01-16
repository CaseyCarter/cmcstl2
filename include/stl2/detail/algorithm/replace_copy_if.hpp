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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_COPY_IF_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_COPY_IF_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_copy_if [alg.replace]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using replace_copy_if_result = __in_out_result<I, O>;

	struct __replace_copy_if_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class T,
			output_iterator<const T&> O, class Proj = identity,
			indirect_unary_predicate<projected<I, Proj>> Pred>
		requires indirectly_copyable<I, O>
		constexpr replace_copy_if_result<I, O>
		operator()(I first, S last, O result, Pred pred,
			const T& new_value, Proj proj = {}) const
		{
			for (; first != last; (void) ++first, (void) ++result) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(pred, __stl2::invoke(proj, v))) {
					*result = new_value;
				} else {
					*result = std::forward<iter_reference_t<I>>(v);
				}
			}
			return {std::move(first), std::move(result)};
		}

		template<input_range R, class T, output_iterator<const T&> O,
			class Proj = identity,
			indirect_unary_predicate<projected<iterator_t<R>, Proj>> Pred>
		requires indirectly_copyable<iterator_t<R>, O>
		constexpr replace_copy_if_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, Pred pred, const T& new_value,
			Proj proj = {}) const
		{
			return (*this)(begin(r), end(r), std::move(result),
				__stl2::ref(pred), new_value, __stl2::ref(proj));
		}
	};

	inline constexpr __replace_copy_if_fn replace_copy_if{};
} STL2_CLOSE_NAMESPACE

#endif
