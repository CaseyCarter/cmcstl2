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
	struct __min_fn : private __niebloid {
	private:
		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		requires Copyable<iter_value_t<iterator_t<R>>>
		static constexpr iter_value_t<iterator_t<R>>
		impl(R&& r, Comp comp = {}, Proj proj = {})
		{
			auto first = begin(r);
			auto last = end(r);
			STL2_EXPECT(first != last);
			iter_value_t<iterator_t<R>> result = *first;
			while (++first != last) {
				auto&& tmp = *first;
				if (__stl2::invoke(comp, __stl2::invoke(proj, tmp), __stl2::invoke(proj, result))) {
					result = (decltype(tmp)&&)tmp;
				}
			}
			return result;
		}
	public:
		template<class T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = less>
		constexpr const T& operator()(const T& a, const T& b, Comp comp = {},
			Proj proj = {}) const
		{
			return __stl2::invoke(comp, __stl2::invoke(proj, b), __stl2::invoke(proj, a)) ? b : a;
		}

		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		requires Copyable<iter_value_t<iterator_t<R>>>
		constexpr iter_value_t<iterator_t<R>>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const
		{
			return __min_fn::impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}

		template<Copyable T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = ranges::less>
		constexpr T operator()(std::initializer_list<T> r,
			Comp comp = {}, Proj proj = {}) const
		{
			return __min_fn::impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __min_fn min {};
} STL2_CLOSE_NAMESPACE

#endif
