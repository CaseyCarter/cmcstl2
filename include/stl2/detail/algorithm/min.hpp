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
			models::Copyable<value_type_t<iterator_t<Rng>>> &&
			models::IndirectCallableStrictWeakOrder<
				__f<Comp>, projected<iterator_t<Rng>, __f<Proj>>>
		constexpr value_type_t<iterator_t<Rng>>
		impl(Rng&& rng, Comp&& comp_ = Comp{}, Proj&& proj_ = Proj{})
		{
			auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
			auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
			auto first = __stl2::begin(rng);
			auto last = __stl2::end(rng);
			STL2_EXPECT(first != last);
			value_type_t<iterator_t<Rng>> result = *first;
			while (++first != last) {
				auto&& tmp = *first;
				if (comp(proj(tmp), proj(result))) {
					result = (decltype(tmp)&&)tmp;
				}
			}
			return result;
		}
	}

	template<class T, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectCallableStrictWeakOrder<
			__f<Comp>, projected<const T*, __f<Proj>>>
	constexpr const T& min(const T& a, const T& b, Comp&& comp_ = Comp{},
		Proj&& proj_ = Proj{})
	{
		auto comp = ext::make_callable_wrapper(__stl2::forward<Comp>(comp_));
		auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
		return comp(proj(b), proj(a)) ? b : a;
	}

	template <InputRange Rng, class Comp = less<>, class Proj = identity>
	requires
		models::Copyable<value_type_t<iterator_t<Rng>>> &&
		models::IndirectCallableStrictWeakOrder<
			__f<Comp>, projected<iterator_t<Rng>, __f<Proj>>>
	STL2_CONSTEXPR_EXT value_type_t<iterator_t<Rng>>
	min(Rng&& rng, Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		return __min::impl(rng, __stl2::forward<Comp>(comp),
			__stl2::forward<Proj>(proj));
	}

	template <Copyable T, class Comp = less<>, class Proj = identity>
	requires
		models::IndirectCallableStrictWeakOrder<
			__f<Comp>, projected<const T*, __f<Proj>>>
	constexpr T min(std::initializer_list<T>&& rng,
		Comp&& comp = Comp{}, Proj&& proj = Proj{})
	{
		return __min::impl(rng, __stl2::forward<Comp>(comp),
			__stl2::forward<Proj>(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
