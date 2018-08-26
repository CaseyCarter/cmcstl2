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
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// rotate_copy [alg.rotate]
//
STL2_OPEN_NAMESPACE {
	template <class I, class O>
	struct rotate_copy_result : __in_out_result<I, O> {};

	struct __rotate_copy_fn {
		template<ForwardIterator I, Sentinel<I> S, WeaklyIncrementable O>
		requires IndirectlyCopyable<I, O>
		constexpr rotate_copy_result<I, O> operator()(I first, I middle, S last, O result) const
		{
			auto res = __stl2::copy(middle, std::move(last), std::move(result));
			res.out = __stl2::copy(std::move(first), std::move(middle), std::move(res.out)).out;
			return {std::move(res.in), std::move(res.out)};
		}

		template<ForwardRange Rng, WeaklyIncrementable O>
		requires IndirectlyCopyable<iterator_t<Rng>, O>
		constexpr rotate_copy_result<safe_iterator_t<Rng>, O>
		operator()(Rng&& rng, iterator_t<Rng> middle, O result) const
		{
			return (*this)(__stl2::begin(rng), std::move(middle), __stl2::end(rng),
				std::move(result));
		}
	};

	inline constexpr __rotate_copy_fn rotate_copy {};
} STL2_CLOSE_NAMESPACE

#endif
