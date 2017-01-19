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
#include <stl2/detail/algorithm/max_element.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// max [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	namespace __max {
		template <InputRange Rng, class Comp, class Proj>
		requires
			models::Copyable<value_type_t<iterator_t<Rng>>> &&
			models::IndirectStrictWeakOrder<
				Comp, projected<iterator_t<Rng>, Proj>>
		constexpr value_type_t<iterator_t<Rng>>
		impl(Rng&& rng, Comp comp, Proj proj)
		{
			auto first = __stl2::begin(rng);
			auto last = __stl2::end(rng);
			STL2_EXPECT(first != last);
			value_type_t<iterator_t<Rng>> result = *first;
			while (++first != last) {
				auto&& tmp = *first;
				if (__stl2::invoke(comp, __stl2::invoke(proj, result), __stl2::invoke(proj, tmp))) {
					result = (decltype(tmp)&&)tmp;
				}
			}
			return result;
		}
	}

	template <class T, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<const T*, Proj>>
	constexpr const T& max(const T& a, const T& b, Comp comp = Comp{},
		Proj proj = Proj{})
	{
		return !__stl2::invoke(comp, __stl2::invoke(proj, a), __stl2::invoke(proj, b)) ? a : b;
	}

	template <InputRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Copyable<value_type_t<iterator_t<Rng>>> &&
		models::IndirectStrictWeakOrder<
			Comp, projected<iterator_t<Rng>, Proj>>
	STL2_CONSTEXPR_EXT value_type_t<iterator_t<Rng>>
	max(Rng&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __max::impl(rng, __stl2::ref(comp),
			__stl2::ref(proj));
	}

	template <Copyable T, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectStrictWeakOrder<
			Comp, projected<const T*, Proj>>
	constexpr T max(std::initializer_list<T>&& rng, Comp comp = Comp{}, Proj proj = Proj{})
	{
		return __max::impl(rng, __stl2::ref(comp), __stl2::ref(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
