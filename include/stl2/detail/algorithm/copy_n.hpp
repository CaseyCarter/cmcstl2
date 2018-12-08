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
#ifndef STL2_DETAIL_ALGORITHM_COPY_N_HPP
#define STL2_DETAIL_ALGORITHM_COPY_N_HPP

#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_n [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using copy_n_result = __in_out_result<I, O>;

	struct __copy_n_fn : private __niebloid {
		template<InputIterator I, WeaklyIncrementable O>
		requires IndirectlyCopyable<I, O>
		constexpr copy_n_result<I, O>
			operator()(I first_, iter_difference_t<I> n, O result) const
		{
			STL2_EXPECT(n >= 0);
			auto norig = n;
			auto first = __stl2::ext::uncounted(first_);
			for(; n > 0; ++first, ++result, --n) {
				*result = *first;
			}
			return {
				__stl2::ext::recounted(first_, first, norig),
				std::move(result)
			};
		}
	};

	inline constexpr __copy_n_fn copy_n {};
} STL2_CLOSE_NAMESPACE

#endif
