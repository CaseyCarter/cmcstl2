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
#ifndef STL2_DETAIL_ALGORITHM_MINMAX_HPP
#define STL2_DETAIL_ALGORITHM_MINMAX_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __minmax_fn : private __niebloid {
	private:
		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		requires Copyable<iter_value_t<iterator_t<R>>>
		static constexpr tagged_pair<tag::min(iter_value_t<iterator_t<R>>),
			tag::max(iter_value_t<iterator_t<R>>)>
		impl(R&& r, Comp comp = {}, Proj proj = {})
		{
			using V = iter_value_t<iterator_t<R>>;
			auto first = begin(r);
			auto last = end(r);
			STL2_EXPECT(first != last);
			auto result = tagged_pair<tag::min(V), tag::max(V)>{
				*first, *first
			};
			if (++first != last) {
				{
					auto&& tmp = *first;
					if (__stl2::invoke(comp, __stl2::invoke(proj, tmp), __stl2::invoke(proj, result.first))) {
						result.first = (decltype(tmp)&&)tmp;
					} else {
						result.second = (decltype(tmp)&&)tmp;
					}
				}
				while (++first != last) {
					auto tmp1 = V{*first};
					if (++first == last) {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.first))) {
							result.first = std::move(tmp1);
						} else if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.second))) {
							result.second = std::move(tmp1);
						}
						break;
					}

					auto&& tmp2 = *first;
					if (__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, tmp1))) {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, result.first))) {
							result.first = (decltype(tmp2)&&)tmp2;
						}
						if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.second))) {
							result.second = std::move(tmp1);
						}
					} else {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.first))) {
							result.first = std::move(tmp1);
						}
						if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, result.second))) {
							result.second = (decltype(tmp2)&&)tmp2;
						}
					}
				}
			}
			return result;
		}
	public:
		template<class T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = less>
		constexpr tagged_pair<tag::min(const T&), tag::max(const T&)>
		operator()(const T& a, const T& b, Comp comp = {}, Proj proj = {}) const
		{
			if (__stl2::invoke(comp, __stl2::invoke(proj, b), __stl2::invoke(proj, a))) {
				return {b, a};
			} else {
				return {a, b};
			}
		}

		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		requires Copyable<iter_value_t<iterator_t<R>>>
		constexpr tagged_pair<tag::min(iter_value_t<iterator_t<R>>),
			tag::max(iter_value_t<iterator_t<R>>)>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const
		{
			return __minmax_fn::impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}

		template<Copyable T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = less>
		constexpr tagged_pair<tag::min(T), tag::max(T)>
		operator()(std::initializer_list<T>&& r, Comp comp = {}, Proj proj = {}) const
		{
			return __minmax_fn::impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __minmax_fn minmax {};
} STL2_CLOSE_NAMESPACE

#endif
