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
	template <class F, class...Args>
	concept bool Invocable =
		requires(F&& f, Args&&...args) {
			__invoke::impl((F&&)f, (Args&&)args...);
		};

	namespace models {
		template <class, class...>
		constexpr bool Invocable = false;
		__stl2::Invocable{F, ...Args}
		constexpr bool Invocable<F, Args...> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// RegularInvocable [concepts.lib.callables.regularcallable]
	//
	template <class F, class...Args>
	concept bool RegularInvocable =
		Invocable<F, Args...>;

	namespace models {
		template <class, class...>
		constexpr bool RegularInvocable = false;
		__stl2::RegularInvocable{F, ...Args}
		constexpr bool RegularInvocable<F, Args...> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Predicate [concepts.lib.callables.predicate]
	//
	template <class F, class...Args>
	concept bool Predicate =
		RegularInvocable<F, Args...> &&
			Boolean<result_of_t<F&&(Args&&...)>>;

	namespace models {
		template <class, class...>
		constexpr bool Predicate = false;
		__stl2::Predicate{F, ...Args}
		constexpr bool Predicate<F, Args...> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Relation [concepts.lib.callables.relation]
	//
	template <class R, class T, class U>
	concept bool Relation =
		Predicate<R, T, T> &&
		Predicate<R, U, U> &&
		Predicate<R, T, U> &&
		Predicate<R, U, T> &&
		CommonReference<
			const remove_reference_t<T>&,
			const remove_reference_t<U>&> &&
		Predicate<
			R,
			common_reference_t<
				const remove_reference_t<T>&,
				const remove_reference_t<U>&>,
			common_reference_t<
				const remove_reference_t<T>&,
				const remove_reference_t<U>&>>;

	namespace models {
		template <class R, class T, class U>
		constexpr bool Relation = false;
		__stl2::Relation{R, T, U}
		constexpr bool Relation<R, T, U> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// StrictWeakOrder [concepts.lib.callables.strictweakorder]
	//
	template <class R, class T, class U>
	concept bool StrictWeakOrder =
		Relation<R, T, U>;

	namespace models {
		template <class R, class T, class U>
		constexpr bool StrictWeakOrder = false;
		__stl2::StrictWeakOrder{R, T, U}
		constexpr bool StrictWeakOrder<R, T, U> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
