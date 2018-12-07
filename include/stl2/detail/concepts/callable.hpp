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

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Indirect callables [indirectfunc.indirectcallables]
	//
	template<class... T>
	struct __common_reference
	: meta::bool_<sizeof...(T) == 1> {};

	template<class T, class U, class... Rest>
	requires
		CommonReference<T, U>
	struct __common_reference<T, U, Rest...>
	: __common_reference<common_reference_t<T, U>, Rest...> {};

	template<Readable... Is>
	using __iter_args_lists =
		meta::push_back<
			meta::cartesian_product<
				meta::list<meta::list<iter_value_t<Is>&, iter_reference_t<Is>>...>>,
			meta::list<iter_common_reference_t<Is>...>>;

	template<typename MapFn, typename ReduceFn>
	using __iter_map_reduce_fn =
		meta::compose<
			meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
			meta::quote<__iter_args_lists>>;

	template<class F, class... Args>
	requires Invocable<F, Args...>
	using __callable_result_t = invoke_result_t<F, Args...>;

	namespace ext {
		template<class F, class... Is>
		STL2_CONCEPT IndirectInvocable =
			(Readable<Is> && ... && true) &&
			CopyConstructible<F> &&
			// The following 3 are checked redundantly, but are called out
			// specifically for better error messages on concept check failure.
			Invocable<F&, iter_value_t<Is>&...> &&
			Invocable<F&, iter_reference_t<Is>...> &&
			Invocable<F&, iter_common_reference_t<Is>...> &&
			// redundantly checks the above 3 requirements
			meta::_v<meta::invoke<
				__iter_map_reduce_fn<
					meta::bind_front<meta::quote<__callable_result_t>, F&>,
					meta::quote<__common_reference>>,
				Is...>>;
	}

	template<class F, class I>
	STL2_CONCEPT IndirectUnaryInvocable =
		ext::IndirectInvocable<F, I>;

	///////////////////////////////////////////////////////////////////////////
	// indirect_result_t
	//
	template<class F, class... Is>
	requires (Readable<Is> && ...) && Invocable<F, iter_reference_t<Is>...>
	using indirect_result_t = invoke_result_t<F, iter_reference_t<Is>&&...>;

	namespace ext {
		template<class F, class... Is>
		STL2_CONCEPT IndirectRegularInvocable =
			IndirectInvocable<F, Is...>;
	}

	template<class F, class I>
	STL2_CONCEPT IndirectRegularUnaryInvocable =
		ext::IndirectRegularInvocable<F, I>;

	template<class, class...> struct __predicate : std::false_type {};
	template<class F, class... Args>
	requires Predicate<F, Args...>
	struct __predicate<F, Args...> : std::true_type {};

	namespace ext {
		template<class F, class... Is>
		STL2_CONCEPT IndirectPredicate =
			(Readable<Is> && ... && true) &&
			CopyConstructible<F> &&
			// The following 3 are checked redundantly, but are called out
			// specifically for better error messages on concept check failure.
			Predicate<F&, iter_value_t<Is>&...> &&
			Predicate<F&, iter_reference_t<Is>...> &&
			Predicate<F&, iter_common_reference_t<Is>...> &&
			// redundantly checks the above 3 requirements
			meta::_v<meta::invoke<
				__iter_map_reduce_fn<
					meta::bind_front<meta::quote<__predicate>, F&>,
					meta::quote<meta::strict_and>>,
				Is...>>;
	}

	template<class F, class I>
	STL2_CONCEPT IndirectUnaryPredicate =
		ext::IndirectPredicate<F, I>;

	template<class F, class I1, class I2 = I1>
	STL2_CONCEPT IndirectRelation =
		Readable<I1> &&
		Readable<I2> &&
		CopyConstructible<F> &&
		Relation<F&, iter_value_t<I1>&, iter_value_t<I2>&> &&
		Relation<F&, iter_value_t<I1>&, iter_reference_t<I2>> &&
		Relation<F&, iter_reference_t<I1>, iter_value_t<I2>&> &&
		Relation<F&, iter_reference_t<I1>, iter_reference_t<I2>> &&
		Relation<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

	template<class F, class I1, class I2 = I1>
	STL2_CONCEPT IndirectStrictWeakOrder =
		Readable<I1> &&
		Readable<I2> &&
		CopyConstructible<F> &&
		StrictWeakOrder<F&, iter_value_t<I1>&, iter_value_t<I2>&> &&
		StrictWeakOrder<F&, iter_value_t<I1>&, iter_reference_t<I2>> &&
		StrictWeakOrder<F&, iter_reference_t<I1>, iter_value_t<I2>&> &&
		StrictWeakOrder<F&, iter_reference_t<I1>, iter_reference_t<I2>> &&
		StrictWeakOrder<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

	///////////////////////////////////////////////////////////////////////////
	// projected [projected.indirectcallables]
	//
	template<Readable I, IndirectRegularUnaryInvocable<I> Proj>
	struct projected {
		using value_type = __uncvref<indirect_result_t<Proj&, I>>;
		indirect_result_t<Proj&, I> operator*() const;
	};

	template<WeaklyIncrementable I, class Proj>
	struct incrementable_traits<projected<I, Proj>> :
		incrementable_traits<I> {};
} STL2_CLOSE_NAMESPACE

#endif
