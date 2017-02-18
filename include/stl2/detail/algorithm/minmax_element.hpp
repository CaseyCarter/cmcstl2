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
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/
//
#ifndef STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP
#define STL2_DETAIL_ALGORITHM_MINMAX_ELEMENT_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	template <ForwardIterator I, Sentinel<I> S, class Comp = less<>, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<I, Proj>>()
	STL2_CONSTEXPR_EXT tagged_pair<tag::min(I), tag::max(I)>
	minmax_element(I first, S last, Comp comp = Comp{}, Proj proj = Proj{})
	{
		auto result = tagged_pair<tag::min(I), tag::max(I)>{first, first};
		if (first == last || ++first == last) {
			return result;
		}
		if (__stl2::invoke(comp, __stl2::invoke(proj, *first), __stl2::invoke(proj, *result.first))) {
			result.first = first;
		} else {
			result.second = first;
		}
		while (++first != last) {
			I tmp = first;
			if (++first == last) {
				if (__stl2::invoke(comp, __stl2::invoke(proj, *tmp), __stl2::invoke(proj, *result.first))) {
					result.first = tmp;
				} else if (!__stl2::invoke(comp, __stl2::invoke(proj, *tmp), __stl2::invoke(proj, *result.second))) {
					result.second = tmp;
				}
				break;
			}

			if (__stl2::invoke(comp, __stl2::invoke(proj, *first), __stl2::invoke(proj, *tmp))) {
				if (__stl2::invoke(comp, __stl2::invoke(proj, *first), __stl2::invoke(proj, *result.first))) {
					result.first = first;
				}
				if (!__stl2::invoke(comp, __stl2::invoke(proj, *tmp), __stl2::invoke(proj, *result.second))) {
					result.second = tmp;
				}
			} else {
				if (__stl2::invoke(comp, __stl2::invoke(proj, *tmp), __stl2::invoke(proj, *result.first))) {
					result.first = tmp;
				}
				if (!__stl2::invoke(comp, __stl2::invoke(proj, *first), __stl2::invoke(proj, *result.second))) {
					result.second = first;
				}
			}
		}
		return result;
	}

	template <ForwardRange Rng, class Comp = less<>, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<iterator_t<Rng>, Proj>>()
	STL2_CONSTEXPR_EXT tagged_pair<tag::min(safe_iterator_t<Rng>),
		tag::max(safe_iterator_t<Rng>)>
	minmax_element(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::minmax_element(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}

	// Extension
	template <class E, class Comp = less<>, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<const E*, Proj>>()
	STL2_CONSTEXPR_EXT tagged_pair<tag::min(dangling<const E*>), tag::max(dangling<const E*>)>
	minmax_element(std::initializer_list<E>&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __stl2::minmax_element(__stl2::begin(rng), __stl2::end(rng),
			__stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
