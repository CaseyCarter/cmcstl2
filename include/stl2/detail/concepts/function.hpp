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
// Invocable Concepts [concepts.lib.callables]
//
STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Invocable [concepts.lib.callables.callable]
	//
	template<class F, class... Args>
	STL2_CONCEPT Invocable =
		requires(F&& f, Args&&... args) {
			__stl2::invoke((F&&)f, (Args&&)args...);
		};

	///////////////////////////////////////////////////////////////////////////
	// RegularInvocable [concepts.lib.callables.regularcallable]
	//
	template<class F, class... Args>
	STL2_CONCEPT RegularInvocable = Invocable<F, Args...>;

	///////////////////////////////////////////////////////////////////////////
	// Predicate [concepts.lib.callables.predicate]
	//
	template<class F, class... Args>
	STL2_CONCEPT Predicate =
		RegularInvocable<F, Args...> && Boolean<invoke_result_t<F, Args...>>;

	///////////////////////////////////////////////////////////////////////////
	// Relation [concepts.lib.callables.relation]
	//
	template<class R, class T, class U>
	STL2_CONCEPT Relation =
		Predicate<R, T, T> &&
		Predicate<R, U, U> &&
		Predicate<R, T, U> &&
		Predicate<R, U, T> &&
		CommonReference<
			const std::remove_reference_t<T>&,
			const std::remove_reference_t<U>&> &&
		Predicate<
			R,
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>,
			common_reference_t<
				const std::remove_reference_t<T>&,
				const std::remove_reference_t<U>&>>;

	///////////////////////////////////////////////////////////////////////////
	// StrictWeakOrder [concepts.lib.callables.strictweakorder]
	//
	template<class R, class T, class U>
	STL2_CONCEPT StrictWeakOrder = Relation<R, T, U>;
} STL2_CLOSE_NAMESPACE

#endif
