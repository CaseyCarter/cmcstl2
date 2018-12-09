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
#ifndef STL2_DETAIL_ALGORITHM_COPY_IF_HPP
#define STL2_DETAIL_ALGORITHM_COPY_IF_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_if [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using copy_if_result = __in_out_result<I, O>;

	struct __copy_if_fn : private __niebloid {
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O, class Proj = identity,
			IndirectUnaryPredicate<projected<I, Proj>> Pred>
		requires IndirectlyCopyable<I, O>
		constexpr copy_if_result<I, O>
		operator()(I first, S last, O result, Pred pred, Proj proj = {}) const
		{
			for (; first != last; ++first) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(pred, __stl2::invoke(proj, v))) {
					*result = std::forward<iter_reference_t<I>>(v);
					++result;
				}
			}

			return {std::move(first), std::move(result)};
		}

		template<InputRange R, WeaklyIncrementable O, class Proj = identity,
			IndirectUnaryPredicate<projected<iterator_t<R>, Proj>> Pred>
		requires IndirectlyCopyable<iterator_t<R>, O>
		constexpr copy_if_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, Pred pred, Proj proj = {}) const
		{
			return (*this)(
				begin(r), end(r),
				std::move(result),
				std::ref(pred),
				std::ref(proj));
		}
	};

	inline constexpr __copy_if_fn copy_if {};
} STL2_CLOSE_NAMESPACE

#endif
