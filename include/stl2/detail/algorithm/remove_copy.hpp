// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_REMOVE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REMOVE_COPY_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// remove_copy [alg.remove]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using remove_copy_result = __in_out_result<I, O>;

	struct __remove_copy_fn : private __niebloid {
		template<input_iterator I, sentinel_for<I> S, weakly_incrementable O, class T,
			class Proj = identity>
		requires indirectly_copyable<I, O> &&
			indirect_relation<equal_to, projected<I, Proj>, const T*>
		constexpr remove_copy_result<I, O>
		operator()(I first, S last, O result, const T& value, Proj proj = {}) const {
			for (; first != last; ++first) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(proj, v) != value) {
					*result = std::forward<iter_reference_t<I>>(v);
					++result;
				}
			}
			return {std::move(first), std::move(result)};
		}

		template<input_range R, weakly_incrementable O, class T, class Proj = identity>
		requires indirectly_copyable<iterator_t<R>, O> &&
			indirect_relation<equal_to, projected<iterator_t<R>, Proj>, const T*>
		constexpr remove_copy_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, const T& value, Proj proj = {}) const {
			return (*this)(begin(r), end(r), std::move(result), value, __stl2::ref(proj));
		}
	};

	inline constexpr __remove_copy_fn remove_copy{};
} STL2_CLOSE_NAMESPACE

#endif
