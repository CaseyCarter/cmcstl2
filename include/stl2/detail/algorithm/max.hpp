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

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// max [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __max_fn : private __niebloid {
		template<class T, class Proj = identity,
			indirect_strict_weak_order<projected<const T*, Proj>> Comp = less>
		constexpr const T&
		operator()(const T& a, const T& b, Comp comp = {}, Proj proj = {}) const {
			const bool test = __stl2::invoke(comp,
				__stl2::invoke(proj, a), __stl2::invoke(proj, b));
			return !test ? a : b;
		}

		template<copyable T, class Proj = identity,
			indirect_strict_weak_order<projected<const T*, Proj>> Comp = less>
		constexpr T operator()(std::initializer_list<T> r, Comp comp = {},
			Proj proj = {}) const
		{
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}

		template<input_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		requires
			indirectly_copyable_storable<iterator_t<R>, iter_value_t<iterator_t<R>>*>
		constexpr iter_value_t<iterator_t<R>>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}
	private:
		template<input_range R, class Proj,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp>
		requires
			indirectly_copyable_storable<iterator_t<R>, iter_value_t<iterator_t<R>>*>
		static constexpr iter_value_t<iterator_t<R>>
		impl(R&& r, Comp comp, Proj proj) {
			auto first = begin(r);
			auto last = end(r);
			STL2_EXPECT(first != last);
			iter_value_t<iterator_t<R>> result = *first;
			while (++first != last) {
				auto&& tmp = *first;
				if (__stl2::invoke(comp,
						__stl2::invoke(proj, result),
						__stl2::invoke(proj, tmp))) {
					result = static_cast<decltype(tmp)>(tmp);
				}
			}
			return result;
		}
	};

	inline constexpr __max_fn max{};
} STL2_CLOSE_NAMESPACE

#endif
