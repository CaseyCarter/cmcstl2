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
#ifndef STL2_DETAIL_CONCEPTS_FUNCTION_HPP
#define STL2_DETAIL_CONCEPTS_FUNCTION_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>

///////////////////////////////////////////////////////////////////////////
// invocable Concepts [concepts.lib.callables]
//
STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// invocable [concepts.lib.callables.callable]
	//
	template<class F, class... Args>
	META_CONCEPT invocable =
		requires(F&& f, Args&&... args) {
			__stl2::invoke((F&&)f, (Args&&)args...);
		};

	///////////////////////////////////////////////////////////////////////////
	// regular_invocable [concepts.lib.callables.regularcallable]
	//
	template<class F, class... Args>
	META_CONCEPT regular_invocable = invocable<F, Args...>;

	///////////////////////////////////////////////////////////////////////////
	// predicate [concepts.lib.callables.predicate]
	//
	template<class F, class... Args>
	META_CONCEPT predicate =
		regular_invocable<F, Args...> && boolean<invoke_result_t<F, Args...>>;

	///////////////////////////////////////////////////////////////////////////
	// relation [concepts.lib.callables.relation]
	//
	template<class R, class T, class U>
	META_CONCEPT relation =
		predicate<R, T, T> &&
		predicate<R, U, U> &&
		predicate<R, T, U> &&
		predicate<R, U, T> &&
		common_reference_with<
			const std::remove_reference_t<T>&,
			const std::remove_reference_t<U>&> &&
		predicate<
			R,
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>,
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>>;

	///////////////////////////////////////////////////////////////////////////
	// strict_weak_order [concepts.lib.callables.strictweakorder]
	//
	template<class R, class T, class U>
	META_CONCEPT strict_weak_order = relation<R, T, U>;
} STL2_CLOSE_NAMESPACE

#endif
