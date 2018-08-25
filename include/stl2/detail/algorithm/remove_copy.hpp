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
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_COPY_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// remove_copy [alg.remove]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	struct remove_copy_result : __in_out_result<I, O> {};

	struct __remove_copy_fn {
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O, class T, class Proj = identity>
		requires IndirectlyCopyable<I, O> &&
			IndirectRelation<ranges::equal_to<>, projected<I, Proj>, const T*>
		constexpr remove_copy_result<I, O>
		operator()(I first, S last, O result, const T& value, Proj proj = Proj{}) const
		{
			for (; first != last; ++first) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(proj, v) != value) {
					*result = std::forward<iter_reference_t<I>>(v);
					++result;
				}
			}
			return {std::move(first), std::move(result)};
		}

		template<InputRange Rng, WeaklyIncrementable O, class T, class Proj = identity>
		requires IndirectlyCopyable<iterator_t<Rng>, O> &&
			IndirectRelation<ranges::equal_to<>, projected<iterator_t<Rng>, Proj>, const T*>
		constexpr remove_copy_result<safe_iterator_t<Rng>, O>
		operator()(Rng&& rng, O result, const T& value, Proj proj = Proj{}) const
		{
			return (*this)(__stl2::begin(rng), __stl2::end(rng), std::move(result),
				value, std::ref(proj));
		}
	};

	inline constexpr __remove_copy_fn remove_copy {};
} STL2_CLOSE_NAMESPACE

#endif
