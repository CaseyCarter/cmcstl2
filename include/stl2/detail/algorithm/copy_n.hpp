// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_COPY_N_HPP
#define STL2_DETAIL_ALGORITHM_COPY_N_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_n [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using copy_n_result = __in_out_result<I, O>;

	struct __copy_n_fn : private __niebloid {
		template<input_iterator I, weakly_incrementable O>
		requires indirectly_copyable<I, O>
		constexpr copy_n_result<I, O>
		operator()(I first_, iter_difference_t<I> n, O result) const {
			if (n < 0) n = 0;
			auto norig = n;
			auto first = ext::uncounted(first_);
			for(; n > 0; (void) ++first, (void) ++result, --n) {
				*result = *first;
			}
			return {
				ext::recounted(first_, first, norig),
				static_cast<O&&>(result)
			};
		}
	};

	inline constexpr __copy_n_fn copy_n{};
} STL2_CLOSE_NAMESPACE

#endif
