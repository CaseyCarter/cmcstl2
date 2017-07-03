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
#include <stl2/detail/fwd.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_n [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, WeaklyIncrementable O>
	requires IndirectlyCopyable<I, O>
	tagged_pair<tag::in(I), tag::out(O)>
	copy_n(I first_, difference_type_t<I> n, O result)
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
} STL2_CLOSE_NAMESPACE

#endif
