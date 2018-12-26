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
#ifndef STL2_DETAIL_ALGORITHM_REVERSE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REVERSE_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse_copy [alg.reverse]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using reverse_copy_result = __in_out_result<I, O>;

	struct __reverse_copy_fn : private __niebloid {
		template<BidirectionalIterator I, Sentinel<I> S, WeaklyIncrementable O>
		requires IndirectlyCopyable<I, O>
		constexpr reverse_copy_result<I, O>
		operator()(I first, S last, O result) const {
			auto bound = next(first, std::move(last));
			for (auto m = bound; m != first; ++result) {
				*result = *--m;
			}
			return {std::move(bound), std::move(result)};
		}

		template<BidirectionalRange R, WeaklyIncrementable O>
		requires IndirectlyCopyable<iterator_t<R>, O>
		constexpr reverse_copy_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result) const {
			return (*this)(begin(r), end(r), std::move(result));
		}
	};

	inline constexpr __reverse_copy_fn reverse_copy {};
} STL2_CLOSE_NAMESPACE

#endif
