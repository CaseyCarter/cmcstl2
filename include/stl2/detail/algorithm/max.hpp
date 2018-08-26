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
#ifndef STL2_DETAIL_ALGORITHM_MAX_HPP
#define STL2_DETAIL_ALGORITHM_MAX_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// max [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	class __max_fn {
		template <InputRange R, class Comp, class Proj>
		requires
			Copyable<iter_value_t<iterator_t<R>>> &&
			IndirectStrictWeakOrder<
				Comp, projected<iterator_t<R>, Proj>>
		static constexpr iter_value_t<iterator_t<R>>
		impl(R&& r, Comp comp, Proj proj)
		{
			auto first = __stl2::begin(r);
			auto last = __stl2::end(r);
			STL2_EXPECT(first != last);
			iter_value_t<iterator_t<R>> result = *first;
			while (++first != last) {
				auto&& tmp = *first;
				if (__invoke::impl(comp, __invoke::impl(proj, result), __invoke::impl(proj, tmp))) {
					result = (decltype(tmp)&&)tmp;
				}
			}
			return result;
		}
	public:
		template<class T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = ranges::less<>>
		constexpr const T& operator()(const T& a, const T& b, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return !__invoke::impl(comp, __invoke::impl(proj, a), __invoke::impl(proj, b)) ? a : b;
		}

		template<Copyable T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = ranges::less<>>
		constexpr T operator()(initializer_list<T> t, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __max_fn::impl(t, std::ref(comp), std::ref(proj));
		}

		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = ranges::less<>>
		requires Copyable<iter_value_t<iterator_t<R>>>
		constexpr iter_value_t<iterator_t<R>>
		operator()(R&& r, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __max_fn::impl(r, std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __max_fn max {};
} STL2_CLOSE_NAMESPACE

#endif
