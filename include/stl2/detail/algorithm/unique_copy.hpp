// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_UNIQUE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_UNIQUE_COPY_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// unique_copy [alg.unique]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using unique_copy_result = __in_out_result<I, O>;

	template<class I, class O>
	META_CONCEPT __unique_copy_helper = input_iterator<I> &&  input_iterator<O> &&
		same_as<iter_value_t<I>, iter_value_t<O>>;

	struct __unique_copy_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, weakly_incrementable O,
			class Proj = identity,
			indirect_relation<projected<I, Proj>> C = equal_to>
		requires indirectly_copyable<I, O> &&
			(forward_iterator<I> || __unique_copy_helper<I, O> ||
			 indirectly_copyable_storable<I, O>)
		constexpr unique_copy_result<I, O> operator()(I first, const S last,
			O result, C comp = {}, Proj proj = {}) const
		{
			if (first != last) {
				auto pred = [&comp, &proj](auto&& lhs, auto&& rhs) {
					return !bool(__stl2::invoke(comp,
						__stl2::invoke(proj, std::forward<decltype(lhs)>(lhs)),
						__stl2::invoke(proj, std::forward<decltype(rhs)>(rhs))));
				};
				if constexpr (forward_iterator<I>) {
					*result = *first;
					++result;
					auto m = first;
					while (++first != last) {
						iter_reference_t<I>&& v = *first;
						if (pred(std::forward<iter_reference_t<I>>(v), *m)) {
							*result = std::forward<iter_reference_t<I>>(v);
							++result;
							m = first;
						}
					}
				} else if constexpr (__unique_copy_helper<I, O>) {
					*result = *first;
					while (++first != last) {
						iter_reference_t<I>&& v = *first;
						if (pred(std::forward<iter_reference_t<I>>(v), *result)) {
							*++result = std::forward<iter_reference_t<I>>(v);
						}
					}
					++result;
				} else { // indirectly_copyable_storable<I, O>
					iter_value_t<I> saved = *first;
					*result = saved;
					++result;
					while (++first != last) {
						iter_reference_t<I>&& v = *first;
						if (pred(std::forward<iter_reference_t<I>>(v), saved)) {
							saved = std::forward<iter_reference_t<I>>(v);
							*result = saved;
							++result;
						}
					}
				}
			}
			return {std::move(first), std::move(result)};
		}

		template<input_range R, weakly_incrementable O, class Proj = identity,
			indirect_relation<projected<iterator_t<R>, Proj>> C = equal_to>
		requires indirectly_copyable<iterator_t<R>, O> &&
			(forward_range<R> || __unique_copy_helper<iterator_t<R>, O> ||
			 indirectly_copyable_storable<iterator_t<R>, O>)
		constexpr unique_copy_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, C comp = {}, Proj proj = {}) const {
			return (*this)(begin(r), end(r), std::move(result),
				__stl2::ref(comp), __stl2::ref(proj));
		}
	};

	inline constexpr __unique_copy_fn unique_copy {};
} STL2_CLOSE_NAMESPACE

#endif
