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
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_backward [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using copy_backward_result = __in_out_result<I, O>;

	struct __copy_backward_fn : private __niebloid {
		template<BidirectionalIterator I1, Sentinel<I1> S1, BidirectionalIterator I2>
		requires IndirectlyCopyable<I1, I2>
		constexpr copy_backward_result<I1, I2>
		operator()(I1 first, S1 sent, I2 out) const
		{
			auto last = next(first, std::move(sent));
			auto i = last;
			while (i != first) {
				*--out = *--i;
			}
			return {std::move(last), std::move(out)};
		}

		template<BidirectionalRange R, BidirectionalIterator I>
		constexpr copy_backward_result<safe_iterator_t<R>, I>
		operator()(R&& r, I result) const
		{
			return (*this)(begin(r), end(r), std::forward<I>(result));
		}
	};

	inline constexpr __copy_backward_fn copy_backward {};
} STL2_CLOSE_NAMESPACE

#endif
