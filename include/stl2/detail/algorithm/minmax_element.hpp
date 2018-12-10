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

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax_element [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __minmax_element_fn : private __niebloid {
		template<ForwardIterator I, Sentinel<I> S, class Proj = identity,
			IndirectStrictWeakOrder<projected<I, Proj>> Comp = less>
		constexpr tagged_pair<tag::min(I), tag::max(I)>
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const
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

		template<ForwardRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		constexpr tagged_pair<tag::min(safe_iterator_t<R>),
			tag::max(safe_iterator_t<R>)>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const
		{
			return (*this)(begin(r), end(r),
				std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __minmax_element_fn minmax_element {};
} STL2_CLOSE_NAMESPACE

#endif
