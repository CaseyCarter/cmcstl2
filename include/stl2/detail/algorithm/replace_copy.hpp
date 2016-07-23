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
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// replace_copy [alg.replace]
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, class T1, class T2,
		OutputIterator<const T2&> O, class Proj = identity>
	requires
		models::IndirectlyCopyable<I, O> &&
		models::IndirectCallableRelation<
			equal_to<>, projected<I, __f<Proj>>, const T1*>
	tagged_pair<tag::in(I), tag::out(O)>
	replace_copy(I first, S last, O result, const T1& old_value,
		const T2& new_value, Proj&& proj_ = Proj{})
	{
		auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));

		for (; first != last; ++first, ++result) {
			reference_t<I>&& v = *first;
			if (proj(v) == old_value) {
				*result = new_value;
			} else {
				*result = __stl2::forward<reference_t<I>>(v);
			}
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <InputRange Rng, class T1, class T2, class O, class Proj = identity>
	requires
		models::OutputIterator<__f<O>, const T2&> &&
		models::IndirectlyCopyable<iterator_t<Rng>, __f<O>> &&
		models::IndirectCallableRelation<
			equal_to<>, projected<iterator_t<Rng>, __f<Proj>>, const T1*>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	replace_copy(Rng&& rng, O&& result, const T1& old_value,
		const T2& new_value, Proj&& proj = Proj{})
	{
		return __stl2::replace_copy(
			__stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result),
			old_value, new_value, __stl2::forward<Proj>(proj));
	}

	// Extension
	template <class E, class T1, class T2, class O, class Proj = identity>
	requires
		models::OutputIterator<__f<O>, const T2&> &&
		models::IndirectlyCopyable<const E*, __f<O>> &&
		models::IndirectCallableRelation<
			equal_to<>, projected<const E*, __f<Proj>>, const T1*>
	tagged_pair<tag::in(dangling<const E*>), tag::out(__f<O>)>
	replace_copy(std::initializer_list<E>&& rng,
		O&& result, const T1& old_value,
		const T2& new_value, Proj&& proj = Proj{})
	{
		return __stl2::replace_copy(
			__stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result),
			old_value, new_value, __stl2::forward<Proj>(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
