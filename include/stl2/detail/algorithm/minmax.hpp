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
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	class __minmax_fn {
		template <InputRange R, class Comp = less<>, class Proj = identity>
		requires
			Copyable<iter_value_t<iterator_t<R>>> &&
			IndirectStrictWeakOrder<
				Comp, projected<iterator_t<R>, Proj>>
		static constexpr minmax_result<iter_value_t<iterator_t<R>>>
		impl(R&& r, Comp comp = Comp{}, Proj proj = Proj{})
		{
			using V = iter_value_t<iterator_t<R>>;
			auto first = __stl2::begin(r);
			auto last = __stl2::end(r);
			STL2_EXPECT(first != last);
			auto result = minmax_result<iter_value_t<iterator_t<R>>>{*first, *first};
			if (++first != last) {
				{
					auto&& tmp = *first;
					if (__invoke::impl(comp, __invoke::impl(proj, tmp), __invoke::impl(proj, result.min))) {
						result.min = (decltype(tmp)&&)tmp;
					} else {
						result.max = (decltype(tmp)&&)tmp;
					}
				}
				while (++first != last) {
					auto tmp1 = V{*first};
					if (++first == last) {
						if (__invoke::impl(comp, __invoke::impl(proj, tmp1), __invoke::impl(proj, result.min))) {
							result.min = std::move(tmp1);
						} else if (!__invoke::impl(comp, __invoke::impl(proj, tmp1), __invoke::impl(proj, result.max))) {
							result.max = std::move(tmp1);
						}
						break;
					}

					auto&& tmp2 = *first;
					if (__invoke::impl(comp, __invoke::impl(proj, tmp2), __invoke::impl(proj, tmp1))) {
						if (__invoke::impl(comp, __invoke::impl(proj, tmp2), __invoke::impl(proj, result.min))) {
							result.min = (decltype(tmp2)&&)tmp2;
						}
						if (!__invoke::impl(comp, __invoke::impl(proj, tmp1), __invoke::impl(proj, result.max))) {
							result.max = std::move(tmp1);
						}
					} else {
						if (__invoke::impl(comp, __invoke::impl(proj, tmp1), __invoke::impl(proj, result.min))) {
							result.min = std::move(tmp1);
						}
						if (!__invoke::impl(comp, __invoke::impl(proj, tmp2), __invoke::impl(proj, result.max))) {
							result.max = (decltype(tmp2)&&)tmp2;
						}
					}
				}
			}
			return result;
		}
	public:
		template<class T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = ranges::less<>>
		constexpr minmax_result<const T&>
		operator()(const T& a, const T& b, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			if (__invoke::impl(comp, __invoke::impl(proj, b), __invoke::impl(proj, a))) {
				return {b, a};
			} else {
				return {a, b};
			}
		}

		template<Copyable T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = ranges::less<>>
		constexpr minmax_result<T>
		operator()(initializer_list<T> t, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __minmax_fn::impl(t, std::ref(comp), std::ref(proj));
		}

		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = ranges::less<>>
		requires Copyable<iter_value_t<iterator_t<R>>>
		constexpr minmax_result<iter_value_t<iterator_t<R>>>
		operator()(R&& r, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __minmax_fn::impl(r, std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __minmax_fn minmax {};
} STL2_CLOSE_NAMESPACE

#endif
