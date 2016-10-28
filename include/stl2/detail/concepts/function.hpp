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
// Callable Concepts [concepts.lib.callables]
//
STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Callable [concepts.lib.callables.callable]
	//
	template <class F, class...Args>
	constexpr bool __callable = false;
	template <class F, class...Args>
	requires
		requires(F&& f, Args&&...args) {
			__invoke::impl((F&&)f, (Args&&)args...);
		}
	constexpr bool __callable<F, Args...> = true;

	template <class F, class...Args>
	concept bool Callable() {
		return __callable<F, Args...>;
	}

	namespace models {
		template <class, class...>
		constexpr bool Callable = false;
		__stl2::Callable{F, ...Args}
		constexpr bool Callable<F, Args...> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// RegularCallable [concepts.lib.callables.regularcallable]
	//
	template <class F, class...Args>
	concept bool RegularCallable() {
		return Callable<F, Args...>();
	}

	namespace models {
		template <class, class...>
		constexpr bool RegularCallable = false;
		__stl2::RegularCallable{F, ...Args}
		constexpr bool RegularCallable<F, Args...> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Predicate [concepts.lib.callables.predicate]
	//
	template <class F, class...Args>
	concept bool Predicate() {
		return RegularCallable<F, Args...>() &&
			Boolean<result_of_t<F(Args...)>>();
	}

	namespace models {
		template <class, class...>
		constexpr bool Predicate = false;
		__stl2::Predicate{F, ...Args}
		constexpr bool Predicate<F, Args...> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Relation [concepts.lib.callables.relation]
	//
	template <class R, class T>
	concept bool Relation() {
		return Predicate<R, T, T>();
	}

	template <class R, class T, class U>
	concept bool Relation() {
		return Relation<R, T>() &&
			Relation<R, U>() &&
			Predicate<R, T, U>() &&
			Predicate<R, U, T>() &&
			CommonReference<const T&, const U&>() &&
			Relation<R, common_reference_t<const T&, const U&>>();
	}

	namespace models {
		template <class R, class T, class U = T>
		constexpr bool Relation = false;
		__stl2::Relation{R, T}
		constexpr bool Relation<R, T, T> = true;
		__stl2::Relation{R, T, U}
		constexpr bool Relation<R, T, U> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// StrictWeakOrder [concepts.lib.callables.strictweakorder]
	//
	template <class R, class T>
	concept bool StrictWeakOrder() {
		return Relation<R, T>();
	}

	template <class R, class T, class U>
	concept bool StrictWeakOrder() {
		return Relation<R, T, U>();
	}

	namespace models {
		template <class R, class T, class U = T>
		constexpr bool StrictWeakOrder = false;
		__stl2::StrictWeakOrder{R, T}
		constexpr bool StrictWeakOrder<R, T, T> = true;
		__stl2::StrictWeakOrder{R, T, U}
		constexpr bool StrictWeakOrder<R, T, U> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
