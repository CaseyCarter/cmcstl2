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
#ifndef STL2_DETAIL_ALGORITHM_ROTATE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_ROTATE_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/results.hpp>

///////////////////////////////////////////////////////////////////////////
// rotate_copy [alg.rotate]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using rotate_copy_result = __in_out_result<I, O>;

	struct __rotate_copy_fn : private __niebloid {
		template<ForwardIterator I, Sentinel<I> S, WeaklyIncrementable O>
		requires IndirectlyCopyable<I, O>
		constexpr rotate_copy_result<I, O>
		operator()(I first, I middle, S last, O result) const
		{
			auto res = copy(middle, std::move(last), std::move(result));
			res.out = copy(std::move(first), std::move(middle),
				std::move(res.out)).out;
			return res;
		}

		template<ForwardRange R, WeaklyIncrementable O>
		requires IndirectlyCopyable<iterator_t<R>, O>
		constexpr rotate_copy_result<safe_iterator_t<R>, O>
		operator()(R&& r, iterator_t<R> middle, O result) const
		{
			return (*this)(
				begin(r), std::move(middle), end(r),
				std::move(result));
		}
	};

	inline constexpr __rotate_copy_fn rotate_copy {};
} STL2_CLOSE_NAMESPACE

#endif
