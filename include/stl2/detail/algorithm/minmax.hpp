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

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	struct __minmax_fn : private __niebloid {
		template<class T, class Proj = identity,
			indirect_strict_weak_order<projected<const T*, Proj>> Comp = less>
		constexpr minmax_result<const T&>
		operator()(const T& a, const T& b, Comp comp = {}, Proj proj = {}) const {
			const bool test = __stl2::invoke(comp,
				__stl2::invoke(proj, b), __stl2::invoke(proj, a));
			if (test)
				return {b, a};
			else
				return {a, b};
		}

		template<copyable T, class Proj = identity,
			indirect_strict_weak_order<projected<const T*, Proj>> Comp = less>
		constexpr minmax_result<T>
		operator()(std::initializer_list<T>&& r, Comp comp = {}, Proj proj = {}) const {
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}

		template<input_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		requires copyable<iter_value_t<iterator_t<R>>>
		constexpr minmax_result<iter_value_t<iterator_t<R>>>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}
	private:
		template<input_range R, class Proj = identity,
			indirect_strict_weak_order<projected<iterator_t<R>, Proj>> Comp = less>
		requires indirectly_copyable_storable<iterator_t<R>, iter_value_t<iterator_t<R>>*>
		static constexpr minmax_result<iter_value_t<iterator_t<R>>>
		impl(R&& r, Comp comp = {}, Proj proj = {}) {
			using V = iter_value_t<iterator_t<R>>;
			auto first = begin(r);
			auto last = end(r);
			STL2_EXPECT(first != last);
			minmax_result<V> result{*first, *first};
			if (++first == last) return result;

			auto pred = [&](auto& lhs, auto& rhs) -> bool {
				return __stl2::invoke(comp, __stl2::invoke(proj, lhs),
					__stl2::invoke(proj, rhs));
			};

			{
				auto&& tmp = *first;
				if (pred(tmp, result.min)) {
					result.min = static_cast<decltype(tmp)>(tmp);
				} else {
					result.max = static_cast<decltype(tmp)>(tmp);
				}
			}

			while (true) {
				if (++first == last) return result;

				auto tmp1 = V{*first};
				if (++first == last) {
					if (pred(tmp1, result.min)) result.min = std::move(tmp1);
					else if (!bool(pred(tmp1, result.max))) result.max = std::move(tmp1);
					return result;
				}

				auto&& tmp2 = *first;
				if (pred(tmp2, tmp1)) {
					if (pred(tmp2, result.min)) {
						result.min = static_cast<decltype(tmp2)>(tmp2);
					}
					if (!bool(pred(tmp1, result.max))) {
						result.max = std::move(tmp1);
					}
				} else {
					if (pred(tmp1, result.min)) {
						result.min = std::move(tmp1);
					}
					if (!bool(pred(tmp2, result.max))) {
						result.max = static_cast<decltype(tmp2)>(tmp2);
					}
				}
			}
		}
	};

	inline constexpr __minmax_fn minmax {};
} STL2_CLOSE_NAMESPACE

#endif
