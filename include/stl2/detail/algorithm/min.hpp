// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_MIN_HPP
#define STL2_DETAIL_ALGORITHM_MIN_HPP

#include <initializer_list>

#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// min [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __min_fn : private __niebloid {
		template<class T, class Proj = identity,
			indirect_strict_weak_order<projected<const T*, Proj>> Comp = less>
		constexpr const T&
		operator()(const T& a, const T& b, Comp comp = {}, Proj proj = {}) const {
			const bool test = __stl2::invoke(comp,
				__stl2::invoke(proj, b), __stl2::invoke(proj, a));
			return test ? b : a;
		}

		template<copyable T, class Proj = identity,
			indirect_strict_weak_order<projected<const T*, Proj>> Comp = less>
		constexpr T
		operator()(std::initializer_list<T> r, Comp comp = {}, Proj proj = {}) const {
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}

		template<input_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		requires indirectly_copyable_storable<iterator_t<R>, iter_value_t<iterator_t<R>>*>
		constexpr iter_value_t<iterator_t<R>>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}
	private:
		template<input_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		requires
			indirectly_copyable_storable<iterator_t<R>, iter_value_t<iterator_t<R>>*>
		static constexpr iter_value_t<iterator_t<R>>
		impl(R&& r, Comp comp = {}, Proj proj = {}) {
			auto first = begin(r);
			auto last = end(r);
			STL2_EXPECT(first != last);
			iter_value_t<iterator_t<R>> result = *first;
			while (++first != last) {
				auto&& tmp = *first;
				if (__stl2::invoke(comp,
						__stl2::invoke(proj, tmp),
						__stl2::invoke(proj, result))) {
					result = static_cast<decltype(tmp)>(tmp);
				}
			}
			return result;
		}
	};

	inline constexpr __min_fn min{};
} STL2_CLOSE_NAMESPACE

#endif
