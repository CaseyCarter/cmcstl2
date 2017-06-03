// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_CALLABLE_HPP
#define STL2_DETAIL_CONCEPTS_CALLABLE_HPP

#include <functional>

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Indirect callables [indirectfunc.indirectcallables]
	//
	template <class...T>
	struct __common_reference
	: meta::bool_<sizeof...(T) == 1> {};

	template <class T, class U, class...Rest>
	requires
		CommonReference<T, U>
	struct __common_reference<T, U, Rest...>
	: __common_reference<common_reference_t<T, U>, Rest...> {};

	template <Readable...Is>
	using __iter_args_lists =
		meta::push_back<
			meta::cartesian_product<
				meta::list<meta::list<value_type_t<Is>&, reference_t<Is>>...>>,
			meta::list<iter_common_reference_t<Is>...>>;

	template <typename MapFn, typename ReduceFn>
	using __iter_map_reduce_fn =
		meta::compose<
			meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
			meta::quote<__iter_args_lists>>;

	Invocable{F, ...Args}
	using __callable_result_t =
		result_of_t<F(Args...)>;

	namespace ext {
		template <class F, class...Is>
		concept bool IndirectInvocable =
			(Readable<Is> &&...&& true) &&
			CopyConstructible<F> &&
			// The following 3 are checked redundantly, but are called out
			// specifically for better error messages on concept check failure.
			Invocable<F&, value_type_t<Is>&...> &&
			Invocable<F&, reference_t<Is>...> &&
			Invocable<F&, iter_common_reference_t<Is>...> &&
			// redundantly checks the above 3 requirements
			meta::_v<meta::invoke<
				__iter_map_reduce_fn<
					meta::bind_front<meta::quote<__callable_result_t>, F&>,
					meta::quote<__common_reference>>,
				Is...>>;
	}

	template <class F, class I>
	concept bool IndirectUnaryInvocable =
		ext::IndirectInvocable<F, I>;

	namespace models {
		template <class, class...>
		constexpr bool IndirectInvocable = false;
		__stl2::ext::IndirectInvocable{F, ...Is}
		constexpr bool IndirectInvocable<F, Is...> = true;

		template <class, class>
		constexpr bool IndirectUnaryInvocable = false;
		__stl2::IndirectUnaryInvocable{F, I}
		constexpr bool IndirectUnaryInvocable<F, I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// indirect_result_of [indirectcallables.indirectfunc]
	//
	template <class>
	struct indirect_result_of {};

	// Not to spec:
	// and https://github.com/ericniebler/stl2/issues/286
	template <class F, class...Is>
	requires
		Invocable<F, reference_t<Is>...>
	struct indirect_result_of<F(Is...)>
	: result_of<F(reference_t<Is>&&...)> {};

	template <class T>
	using indirect_result_of_t =
		meta::_t<indirect_result_of<T>>;

	namespace ext {
		template <class F, class...Is>
		concept bool IndirectRegularInvocable =
			IndirectInvocable<F, Is...>;
	}

	template <class F, class I>
	concept bool IndirectRegularUnaryInvocable =
		ext::IndirectRegularInvocable<F, I>;

	namespace models {
		template <class, class...>
		constexpr bool IndirectRegularInvocable = false;
		__stl2::ext::IndirectRegularInvocable{F, ...Is}
		constexpr bool IndirectRegularInvocable<F, Is...> = true;

		template <class, class>
		constexpr bool IndirectRegularUnaryInvocable = false;
		__stl2::IndirectRegularUnaryInvocable{F, I}
		constexpr bool IndirectRegularInvocable<F, I> = true;
	}

	template <class, class...> struct __predicate : false_type {};
	Predicate{F, ...Args} struct __predicate<F, Args...> : true_type {};

	namespace ext {
		template <class F, class...Is>
		concept bool IndirectPredicate =
			(Readable<Is> &&...&& true) &&
			CopyConstructible<F> &&
			// The following 3 are checked redundantly, but are called out
			// specifically for better error messages on concept check failure.
			Predicate<F&, value_type_t<Is>&...> &&
			Predicate<F&, reference_t<Is>...> &&
			Predicate<F&, iter_common_reference_t<Is>...> &&
			// redundantly checks the above 3 requirements
			meta::_v<meta::invoke<
				__iter_map_reduce_fn<
					meta::bind_front<meta::quote<__predicate>, F&>,
					meta::quote<meta::strict_and>>,
				Is...>>;
	}

	template <class F, class I>
	concept bool IndirectUnaryPredicate =
		ext::IndirectPredicate<F, I>;

	template <class F, class I1, class I2>
	concept bool IndirectBinaryPredicate =
		ext::IndirectPredicate<F, I1, I2>;

	namespace models {
		template <class, class...>
		constexpr bool IndirectPredicate = false;
		__stl2::ext::IndirectPredicate{F, ...Is}
		constexpr bool IndirectPredicate<F, Is...> = true;

		template <class, class>
		constexpr bool IndirectUnaryPredicate = false;
		__stl2::IndirectUnaryPredicate{F, I}
		constexpr bool IndirectUnaryPredicate<F, I> = true;

		template <class, class, class>
		constexpr bool IndirectBinaryPredicate = false;
		__stl2::IndirectBinaryPredicate{F, I1, I2}
		constexpr bool IndirectPredicate<F, I1, I2> = true;
	}

	template <class F, class I1, class I2 = I1>
	concept bool IndirectRelation =
		Readable<I1> &&
		Readable<I2> &&
		CopyConstructible<F> &&
		Relation<F&, value_type_t<I1>&, value_type_t<I2>&> &&
		Relation<F&, value_type_t<I1>&, reference_t<I2>> &&
		Relation<F&, reference_t<I1>, value_type_t<I2>&> &&
		Relation<F&, reference_t<I1>, reference_t<I2>> &&
		Relation<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

	namespace models {
		template <class F, class I1, class I2 = I1>
		constexpr bool IndirectRelation = false;
		__stl2::IndirectRelation{F, I1, I2}
		constexpr bool IndirectRelation<F, I1, I2> = true;
	}

	template <class F, class I1, class I2 = I1>
	concept bool IndirectStrictWeakOrder =
		Readable<I1> &&
		Readable<I2> &&
		CopyConstructible<F> &&
		StrictWeakOrder<F&, value_type_t<I1>&, value_type_t<I2>&> &&
		StrictWeakOrder<F&, value_type_t<I1>&, reference_t<I2>> &&
		StrictWeakOrder<F&, reference_t<I1>, value_type_t<I2>&> &&
		StrictWeakOrder<F&, reference_t<I1>, reference_t<I2>> &&
		StrictWeakOrder<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

	namespace models {
		template <class F, class I1, class I2 = I1>
		constexpr bool IndirectStrictWeakOrder = false;
		__stl2::IndirectStrictWeakOrder{F, I1, I2}
		constexpr bool IndirectStrictWeakOrder<F, I1, I2> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// projected [projected.indirectcallables]
	//
	template <Readable I, IndirectRegularUnaryInvocable<I> Proj>
	struct projected {
		using value_type = __uncvref<indirect_result_of_t<Proj&(I)>>;
		indirect_result_of_t<Proj&(I)> operator*() const;
	};

	template <WeaklyIncrementable I, class Proj>
	struct difference_type<projected<I, Proj>> :
		difference_type<I> {};
} STL2_CLOSE_NAMESPACE

#endif
