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
#ifndef STL2_DETAIL_ALGORITHM_FOR_EACH_HPP
#define STL2_DETAIL_ALGORITHM_FOR_EACH_HPP

#include <initializer_list>
#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// for_each [alg.for_each]
// Not to spec: with the proxy iterator changes in place, IndirectCallable
//   is severely overconstraining for for_each:
//   * functions that accept the reference type for a mutable "lvalue"
//     sequence are rejected
//   * functions that do accept all of IndirectCallable's required
//     parameter types but don't return types with a common reference are
//     rejected despite the fact that for_each is guaranteed to only ever
//     call f(proj(*i)) and discard the return value.
//
STL2_OPEN_NAMESPACE {
	template <InputIterator I, Sentinel<I> S, class F, class Proj = identity>
	requires
		models::IndirectCallable<
			__f<F>, projected<I, __f<Proj>>>
	tagged_pair<tag::in(I), tag::fun(__f<F>)>
	for_each(I first, S last, F&& fun_, Proj&& proj_ = Proj{})
	{
		auto fun = ext::make_callable_wrapper(__stl2::forward<F>(fun_));
		auto proj = ext::make_callable_wrapper(__stl2::forward<Proj>(proj_));
		for (; first != last; ++first) {
			static_cast<void>(fun(proj(*first)));
		}
		return {__stl2::move(first), ext::callable_unwrapper(__stl2::move(fun))};
	}

	template <InputRange Rng, class F, class Proj = identity>
	requires
		models::IndirectCallable<
			__f<F>, projected<iterator_t<Rng>, __f<Proj>>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::fun(__f<F>)>
	for_each(Rng&& rng, F&& f, Proj&& proj = Proj{})
	{
		return __stl2::for_each(__stl2::begin(rng), __stl2::end(rng),
			__stl2::forward<F>(f), __stl2::forward<Proj>(proj));
	}

	// Extension
	template <class E, class F, class Proj = identity>
	requires
		models::IndirectCallable<
			__f<F>, projected<const E*, __f<Proj>>>
	tagged_pair<tag::in(dangling<const E*>), tag::fun(__f<F>)>
	for_each(std::initializer_list<E>&& il, F&& f, Proj&& proj = Proj{})
	{
		return __stl2::for_each(il.begin(), il.end(),
			__stl2::forward<F>(f), __stl2::forward<Proj>(proj));
	}
} STL2_CLOSE_NAMESPACE

#endif
