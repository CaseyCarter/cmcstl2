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
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// rotate_copy [alg.rotate]
//
STL2_OPEN_NAMESPACE {
	template <ForwardIterator I, class F, class S, class O>
	requires
		Same<I, __f<F>> &&
		Sentinel<__f<S>, I> &&
		WeaklyIncrementable<__f<O>> &&
		IndirectlyCopyable<I, __f<O>>
	tagged_pair<tag::in(I), tag::out(__f<O>)>
	rotate_copy(F&& first, I middle, S&& last, O&& out)
	{
		auto res = __stl2::copy(middle, std::forward<S>(last), std::forward<O>(out));
		res.out() = __stl2::copy(std::forward<F>(first), std::move(middle),
			std::move(res.out())).out();
		return res;
	}

	template <ForwardRange Rng, class M, class O>
	requires
		Same<iterator_t<Rng>, __f<M>> &&
		WeaklyIncrementable<__f<O>> &&
		IndirectlyCopyable<iterator_t<Rng>, __f<O>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	rotate_copy(Rng&& rng, M&& middle, O&& result)
	{
		return __stl2::rotate_copy(__stl2::begin(rng), std::forward<M>(middle),
			__stl2::end(rng), std::forward<O>(result));
	}
} STL2_CLOSE_NAMESPACE

#endif
