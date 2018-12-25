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
#ifndef STL2_DETAIL_ALGORITHM_REPLACE_COPY_HPP
#define STL2_DETAIL_ALGORITHM_REPLACE_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_copy [alg.replace]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using replace_copy_result = __in_out_result<I, O>;

	struct __replace_copy_fn : private __niebloid {
		template<InputIterator I, Sentinel<I> S, class T1, class T2,
			OutputIterator<const T2&> O, class Proj = identity>
		requires IndirectlyCopyable<I, O> &&
			IndirectRelation<equal_to, projected<I, Proj>, const T1*>
		constexpr replace_copy_result<I, O>
		operator()(I first, S last, O result, const T1& old_value,
			const T2& new_value, Proj proj = {}) const
		{
			for (; first != last; (void) ++first, (void) ++result) {
				iter_reference_t<I>&& v = *first;
				if (__stl2::invoke(proj, v) == old_value) {
					*result = new_value;
				} else {
					*result = std::forward<iter_reference_t<I>>(v);
				}
			}
			return {std::move(first), std::move(result)};
		}

		template<InputRange R, class T1, class T2, OutputIterator<const T2&> O,
			class Proj = identity>
		requires IndirectlyCopyable<iterator_t<R>, O> &&
			IndirectRelation<equal_to, projected<iterator_t<R>, Proj>, const T1*>
		constexpr replace_copy_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result, const T1& old_value, const T2& new_value,
			Proj proj = {}) const
		{
			return (*this)(begin(r), end(r), std::move(result),
				old_value, new_value, __stl2::ref(proj));
		}
	};

	inline constexpr __replace_copy_fn replace_copy {};
} STL2_CLOSE_NAMESPACE

#endif
