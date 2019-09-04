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
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
	////////////////////////////////////////////////////////////////////////////
	// Indirect callables [indirectcallable.indirectinvocable]
	//
	template<class... T>
	struct __common_reference
	: meta::bool_<sizeof...(T) == 1> {};

	template<class T, class U, class... Rest>
	requires common_reference_with<T, U>
	struct __common_reference<T, U, Rest...>
	: __common_reference<common_reference_t<T, U>, Rest...> {};

	template<readable... Is>
	using __iter_args_lists_ =
		meta::push_back<
			meta::cartesian_product<
				meta::list<meta::list<iter_value_t<Is>&, iter_reference_t<Is>>...>>,
			meta::list<iter_common_reference_t<Is>...>>;
	template<class... Is>
	using __iter_args_lists = __iter_args_lists_<Is...>;

	template<typename MapFn, typename ReduceFn>
	using __iter_map_reduce_fn =
		meta::compose<
			meta::uncurry<meta::on<ReduceFn, meta::uncurry<MapFn>>>,
			meta::quote<__iter_args_lists>>;

	template<class F, class... Args>
	requires invocable<F, Args...>
	using __callable_result_t = invoke_result_t<F, Args...>;

	namespace ext {
		template<class F, class... Is>
		META_CONCEPT indirect_invocable =
			(readable<Is> && ... && true) &&
			copy_constructible<F> &&
			// The following 3 are checked redundantly, but are called out
			// specifically for better error messages on concept check failure.
			invocable<F&, iter_value_t<Is>&...> &&
			invocable<F&, iter_reference_t<Is>...> &&
			invocable<F&, iter_common_reference_t<Is>...> &&
			// redundantly checks the above 3 requirements
			meta::_v<meta::invoke<
				__iter_map_reduce_fn<
					meta::bind_front<meta::quote<__callable_result_t>, F&>,
					meta::quote<__common_reference>>,
				Is...>>;
	}

	template<class F, class I>
	META_CONCEPT indirect_unary_invocable =
		ext::indirect_invocable<F, I>;

	////////////////////////////////////////////////////////////////////////////
	// indirect_result_t
	//
	template<class F, class... Is>
	requires (readable<Is> && ...) && invocable<F, iter_reference_t<Is>...>
	using indirect_result_t = invoke_result_t<F, iter_reference_t<Is>&&...>;

	namespace ext {
		template<class F, class... Is>
		META_CONCEPT indirect_regular_invocable =
			indirect_invocable<F, Is...>;
	}

	template<class F, class I>
	META_CONCEPT indirect_regular_unary_invocable =
		ext::indirect_regular_invocable<F, I>;

	template<class, class...> struct __predicate : std::false_type {};
	template<class F, class... Args>
	requires predicate<F, Args...>
	struct __predicate<F, Args...> : std::true_type {};

	namespace ext {
		template<class F, class... Is>
		META_CONCEPT indirect_predicate =
			(readable<Is> && ... && true) &&
			copy_constructible<F> &&
			// The following 3 are checked redundantly, but are called out
			// specifically for better error messages on concept check failure.
			predicate<F&, iter_value_t<Is>&...> &&
			predicate<F&, iter_reference_t<Is>...> &&
			predicate<F&, iter_common_reference_t<Is>...> &&
			// redundantly checks the above 3 requirements
			meta::_v<meta::invoke<
				__iter_map_reduce_fn<
					meta::bind_front<meta::quote<__predicate>, F&>,
					meta::quote<meta::strict_and>>,
				Is...>>;
	}

	template<class F, class I>
	META_CONCEPT indirect_unary_predicate =
		ext::indirect_predicate<F, I>;

	template<class F, class I1, class I2 = I1>
	META_CONCEPT indirect_relation =
		readable<I1> &&
		readable<I2> &&
		copy_constructible<F> &&
		relation<F&, iter_value_t<I1>&, iter_value_t<I2>&> &&
		relation<F&, iter_value_t<I1>&, iter_reference_t<I2>> &&
		relation<F&, iter_reference_t<I1>, iter_value_t<I2>&> &&
		relation<F&, iter_reference_t<I1>, iter_reference_t<I2>> &&
		relation<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

	template<class F, class I1, class I2 = I1>
	META_CONCEPT indirect_strict_weak_order =
		readable<I1> &&
		readable<I2> &&
		copy_constructible<F> &&
		strict_weak_order<F&, iter_value_t<I1>&, iter_value_t<I2>&> &&
		strict_weak_order<F&, iter_value_t<I1>&, iter_reference_t<I2>> &&
		strict_weak_order<F&, iter_reference_t<I1>, iter_value_t<I2>&> &&
		strict_weak_order<F&, iter_reference_t<I1>, iter_reference_t<I2>> &&
		strict_weak_order<F&, iter_common_reference_t<I1>, iter_common_reference_t<I2>>;

	////////////////////////////////////////////////////////////////////////////
	// projected [projected.indirectcallables]
	//
	template<readable I, indirect_regular_unary_invocable<I> Proj>
	struct projected {
		using value_type = __uncvref<indirect_result_t<Proj&, I>>;
		indirect_result_t<Proj&, I> operator*() const;
	};

	template<weakly_incrementable I, class Proj>
	struct incrementable_traits<projected<I, Proj>> {
		using type = iter_difference_t<I>;
	};

	////////////////////////////////////////////////////////////////////////////
	// indirectly_comparable [alg.req.ind.cmp]
	//
	template<class I1, class I2, class R = equal_to, class P1 = identity,
		class P2 = identity>
	META_CONCEPT indirectly_comparable =
		indirect_relation<R, projected<I1, P1>, projected<I2, P2>>;

	////////////////////////////////////////////////////////////////////////////
	// permutable [alg.req.permutable]
	//
	template<class I>
	META_CONCEPT permutable = forward_iterator<I> &&
		indirectly_movable_storable<I, I> && indirectly_swappable<I, I>;

	////////////////////////////////////////////////////////////////////////////
	// mergeable [alg.req.mergeable]
	//
	template<class I1, class I2, class Out, class R = less,
		class P1 = identity, class P2 = identity>
	META_CONCEPT mergeable = input_iterator<I1> && input_iterator<I2> &&
		weakly_incrementable<Out> &&
		indirectly_copyable<I1, Out> && indirectly_copyable<I2, Out> &&
		indirect_strict_weak_order<R, projected<I1, P1>, projected<I2, P2>>;

	////////////////////////////////////////////////////////////////////////////
	// sortable [alg.req.sortable]
	//
	template<class I, class R = less, class P = identity>
	META_CONCEPT sortable = permutable<I> &&
		indirect_strict_weak_order<R, projected<I, P>>;
} STL2_CLOSE_NAMESPACE

#endif
