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
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// minmax [alg.min.max]
//
STL2_OPEN_NAMESPACE {
	template<class T>
	struct minmax_result {
		T min;
		T max;
	};

	struct __minmax_fn : private __niebloid {
	private:
		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		requires IndirectlyCopyableStorable<iterator_t<R>, iter_value_t<iterator_t<R>>*>
		static constexpr minmax_result<iter_value_t<iterator_t<R>>>
		impl(R&& r, Comp comp = {}, Proj proj = {}) {
			using V = iter_value_t<iterator_t<R>>;
			auto first = begin(r);
			auto last = end(r);
			STL2_EXPECT(first != last);
			minmax_result<V> result{*first, *first};
			if (++first != last) {
				{
					auto&& tmp = *first;
					if (__stl2::invoke(comp, __stl2::invoke(proj, tmp), __stl2::invoke(proj, result.min))) {
						result.min = static_cast<decltype(tmp)>(tmp);
					} else {
						result.max = static_cast<decltype(tmp)>(tmp);
					}
				}
				while (++first != last) {
					auto tmp1 = V{*first};
					if (++first == last) {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.min))) {
							result.min = std::move(tmp1);
						} else if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.max))) {
							result.max = std::move(tmp1);
						}
						break;
					}

					auto&& tmp2 = *first;
					if (__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, tmp1))) {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, result.min))) {
							result.min = static_cast<decltype(tmp2)>(tmp2);
						}
						if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.max))) {
							result.max = std::move(tmp1);
						}
					} else {
						if (__stl2::invoke(comp, __stl2::invoke(proj, tmp1), __stl2::invoke(proj, result.min))) {
							result.min = std::move(tmp1);
						}
						if (!__stl2::invoke(comp, __stl2::invoke(proj, tmp2), __stl2::invoke(proj, result.max))) {
							result.max = static_cast<decltype(tmp2)>(tmp2);
						}
					}
				}
			}
			return result;
		}
	public:
		template<class T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = less>
		constexpr minmax_result<const T&>
		operator()(const T& a, const T& b, Comp comp = {}, Proj proj = {}) const {
			const bool test = __stl2::invoke(comp,
				__stl2::invoke(proj, b), __stl2::invoke(proj, a));
			if (test)
				return {b, a};
			else
				return {a, b};
		}

		template<Copyable T, class Proj = identity,
			IndirectStrictWeakOrder<projected<const T*, Proj>> Comp = less>
		constexpr minmax_result<T>
		operator()(std::initializer_list<T>&& r, Comp comp = {}, Proj proj = {}) const {
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}

		template<InputRange R, class Proj = identity,
			IndirectStrictWeakOrder<projected<iterator_t<R>, Proj>> Comp = less>
		requires Copyable<iter_value_t<iterator_t<R>>>
		constexpr minmax_result<iter_value_t<iterator_t<R>>>
		operator()(R&& r, Comp comp = {}, Proj proj = {}) const {
			return impl(r, __stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __minmax_fn minmax {};
} STL2_CLOSE_NAMESPACE

#endif
