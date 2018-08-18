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
#ifndef STL2_DETAIL_TUPLE_LIKE_HPP
#define STL2_DETAIL_TUPLE_LIKE_HPP

#include <initializer_list>
#include <tuple>

STL2_OPEN_NAMESPACE {
	namespace detail {
		namespace adl {
			using std::get;

			template <std::size_t I, class T>
			constexpr decltype(auto) adl_get(T&& t)
			STL2_NOEXCEPT_RETURN(
				get<I>(static_cast<T&&>(t))
			)
		} // namespace adl
		using adl::adl_get;

		template<class Tup>
		using tuple_indices_t =
			meta::make_index_sequence<
				std::tuple_size<typename std::remove_reference<Tup>::type>::value>;

		class __tuple_transform_fn {
			template <class T, class Fun, size_t... Is>
			static constexpr decltype(auto) impl1(T&& t, Fun& fun, index_sequence<Is...>)
			STL2_NOEXCEPT_RETURN(
				tuple(fun(adl_get<Is>(static_cast<T&&>(t)))...)
			)

			template <class T1, class T2, class Fun, size_t... Is>
			static constexpr decltype(auto) impl2(T1&& t1, T2&& t2, Fun& fun, index_sequence<Is...>)
			STL2_NOEXCEPT_RETURN(
				tuple(fun(adl_get<Is>(static_cast<T1&&>(t1)), adl_get<Is>(static_cast<T2&>(t2)))...)
			)
		public:
			template <class T, class Fun>
			constexpr decltype(auto) operator()(T&& t, Fun fun) const
			STL2_NOEXCEPT_RETURN(
				__tuple_transform_fn::impl1(static_cast<T&&>(t), fun, tuple_indices_t<T>{})
			)

			template <class T1, class T2, class Fun>
			constexpr decltype(auto) operator()(T1&& t1, T2&& t2, Fun fun) const
			STL2_NOEXCEPT_RETURN(
				__tuple_transform_fn::impl2(static_cast<T1&&>(t1), static_cast<T2&&>(t2), fun,
					tuple_indices_t<T1>{})
			)
		};

		inline constexpr __tuple_transform_fn tuple_transform {};

		class __tuple_foldl_fn {
			template <class T, class Val, class Fun>
			static constexpr Val impl(T&&, Val val, Fun&)
			{ return val; }

			template <size_t I0, size_t... Is, class T, class Val, class Fun,
				class Impl = __tuple_foldl_fn>
			static constexpr decltype(auto) impl(T&& t, Val val, Fun& fun)
			STL2_NOEXCEPT_RETURN(
				Impl::template impl<Is...>(static_cast<T&&>(t),
					fun(std::move(val), adl_get<I0>(static_cast<T&&>(t))), fun)
			)

			template <class T, class Val, class Fun, size_t... Is>
			static constexpr decltype(auto) impl2(T&& t, Val val, Fun& fun, index_sequence<Is...>)
			STL2_NOEXCEPT_RETURN(
				__tuple_foldl_fn::impl<Is...>(static_cast<T&&>(t), std::move(val), fun)
			)
		public:
			template <class T, class Val, class Fun>
			constexpr decltype(auto) operator()(T&& t, Val val, Fun fun) const
			STL2_NOEXCEPT_RETURN(
				__tuple_foldl_fn::impl2(static_cast<T&&>(t), std::move(val), fun, tuple_indices_t<T>{})
			)
		};

		inline constexpr __tuple_foldl_fn tuple_foldl {};

		class __tuple_for_each_fn {
			template<class T, class Fun, size_t... Is>
			static constexpr void impl(T&& t, Fun& fun, index_sequence<Is...>)
			{
				(void)std::initializer_list<int>{
					((void)fun(adl_get<Is>(static_cast<T&&>(t))), 42)...};
			}
		public:
			template<class T, class Fun>
			constexpr Fun operator()(T&& t, Fun fun) const
			{
				__tuple_for_each_fn::impl(static_cast<T&&>(t), fun, tuple_indices_t<T>{});
				return fun;
			}
		};

		inline constexpr __tuple_for_each_fn tuple_for_each {};

		class __tuple_min_fn {
			template<class T, class Comp, size_t... Is>
			static constexpr auto impl(T&& t, Comp& comp, index_sequence<Is...>)
			{
				using common_type = common_type_t<tuple_element_t<Is, T>...>;
				auto sequence = std::initializer_list<common_type>{adl_get<Is>(static_cast<T&&>(t))...};
				auto min = sequence.begin();

				// can't include <stl2/detail/algorithm/min.hpp> because it's apparently dependent on
				// this file.
				for (auto i = sequence.begin(); i != sequence.end(); ++i) {
					if (comp(*i, *min)) {
						min = i;
					}
				}

				return *min;
			}
		public:
			template<class T, class Comp = std::less<>>
			constexpr auto operator()(T&& t, Comp comp = Comp{}) const
			{
				return __tuple_min_fn::impl(static_cast<T&&>(t), comp, tuple_indices_t<T>{});
			}
		};

		inline constexpr __tuple_min_fn tuple_min {};
	} // namespace detail
} STL2_CLOSE_NAMESPACE

#endif
