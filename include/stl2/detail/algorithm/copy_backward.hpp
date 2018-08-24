// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP
#define STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP

#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_backward [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template <class I1, class I2>
	struct copy_backward_result : __in_out_result<I1, I2> {};

	struct __copy_backward_fn {
		template<BidirectionalIterator I1, Sentinel<I1> S1, BidirectionalIterator I2>
		requires IndirectlyCopyable<I1, I2>
		constexpr copy_backward_result<I1, I2> operator()(I1 first, S1 last, I2 result) const
		{
			auto end = __stl2::next(first, std::move(last));
			auto i = end;
			while (i != first) {
				*--result = *--i;
			}
			return {std::move(end), std::move(result)};
		}

		template<BidirectionalRange Rng, BidirectionalIterator I>
		requires IndirectlyCopyable<iterator_t<Rng>, I>
		constexpr copy_backward_result<safe_iterator_t<Rng>, I> operator()(Rng&& rng, I result) const
		{
			return (*this)(
				__stl2::begin(rng), __stl2::end(rng),
				std::forward<I>(result));
		}
	};

	inline constexpr __copy_backward_fn copy_backward {};
} STL2_CLOSE_NAMESPACE

#endif
