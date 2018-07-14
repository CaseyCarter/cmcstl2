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
#ifndef STL2_DETAIL_ALGORITHM_MIN_HPP
#define STL2_DETAIL_ALGORITHM_MIN_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// min [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	namespace __min {
		template <InputRange Rng, class Comp = less<>, class Proj = identity>
		requires
			Copyable<iter_value_t<iterator_t<Rng>>> &&
			IndirectStrictWeakOrder<
				Comp, projected<iterator_t<Rng>, Proj>>
		constexpr iter_value_t<iterator_t<Rng>>
		impl(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
		{
			auto first = __stl2::begin(rng);
			auto last = __stl2::end(rng);
			STL2_EXPECT(first != last);
			iter_value_t<iterator_t<Rng>> result = *first;
			while (++first != last) {
				auto&& tmp = *first;
				if (__invoke::impl(comp, __invoke::impl(proj, tmp), __invoke::impl(proj, result))) {
					result = (decltype(tmp)&&)tmp;
				}
			}
			return result;
		}
	}

	template <class T, class Comp = less<>, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<const T*, Proj>>
	constexpr const T& min(const T& a, const T& b, Comp comp = Comp{},
		Proj proj = Proj{})
	{
		return __invoke::impl(comp, __invoke::impl(proj, b), __invoke::impl(proj, a)) ? b : a;
	}

	template <InputRange Rng, class Comp = less<>, class Proj = identity>
	requires
		Copyable<iter_value_t<iterator_t<Rng>>> &&
		IndirectStrictWeakOrder<
			Comp, projected<iterator_t<Rng>, Proj>>
	STL2_CONSTEXPR_EXT iter_value_t<iterator_t<Rng>>
	min(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __min::impl(rng, std::ref(comp), std::ref(proj));
	}

	template <Copyable T, class Comp = less<>, class Proj = identity>
	requires
		IndirectStrictWeakOrder<
			Comp, projected<const T*, Proj>>
	constexpr T min(std::initializer_list<T>&& rng,
		Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __min::impl(rng, std::ref(comp), std::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
