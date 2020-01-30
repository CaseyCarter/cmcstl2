// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_COPY_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_copy [alg.replace]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using replace_copy_result = __in_out_result<I, O>;

	struct __replace_copy_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, class T1, class T2,
			output_iterator<const T2&> O, class Proj = identity>
		requires indirectly_copyable<I, O> &&
			indirect_relation<equal_to, projected<I, Proj>, const T1*>
		constexpr replace_copy_result<I, O>
		operator()(I first, S last, O result, const T1& old_value,
			const T2& new_value, Proj proj = {}) const
		{
			for (; first != last; (void) ++first, (void) ++result) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(proj, v) == old_value) {
					*result = new_value;
				} else {
					*result = std::forward<iter_reference_t<I>>(v);
				}
			}
			return {std::move(first), std::move(result)};
		}

		template<input_range R, class T1, class T2, output_iterator<const T2&> O,
			class Proj = identity>
		requires indirectly_copyable<iterator_t<R>, O> &&
			indirect_relation<equal_to, projected<iterator_t<R>, Proj>, const T1*>
		constexpr replace_copy_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, const T1& old_value, const T2& new_value,
			Proj proj = {}) const
		{
			return (*this)(begin(r), end(r), std::move(result),
				old_value, new_value, __stl2::ref(proj));
		}
	};

	inline constexpr __replace_copy_fn replace_copy{};
} STL2_CLOSE_NAMESPACE

#endif
