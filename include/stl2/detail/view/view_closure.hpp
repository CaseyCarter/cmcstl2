// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_VIEW_CLOSURE_HPP
#define STL2_DETAIL_VIEW_CLOSURE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		struct __pipeable_base {};

		template <class T>
		concept bool Pipeable =
			std::is_base_of_v<__pipeable_base, T> &&
			CopyConstructible<T>;

		template <std::size_t N, std::size_t M>
		concept bool Index = (N == M);

		template <std::size_t N, class T>
		struct __box {
			T value_;
			template <Index<N> M> constexpr T&& get() && noexcept
			{ return static_cast<T&&>(value_); }
			template <Index<N> M> constexpr T& get() & noexcept
			{ return value_; }
			template <Index<N> M> constexpr const T& get() const & noexcept
			{ return value_; }
		};

		template <class Derived>
		struct __pipeable : __pipeable_base {
			template <class Rng>
			friend constexpr auto operator|(Rng&& rng, Derived&& c)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(std::move(c)(std::forward<Rng>(rng)))
			{ return std::move(c)(std::forward<Rng>(rng)); }

			template <class Rng>
			friend constexpr auto operator|(Rng&& rng, Derived& c)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(c(std::forward<Rng>(rng)))
			{ return c(std::forward<Rng>(rng)); }

			template <class Rng>
			friend constexpr auto operator|(Rng&& rng, const Derived& c)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(c(std::forward<Rng>(rng)))
			{ return c(std::forward<Rng>(rng)); }
		};

		template <class Indices, class Fn, class... Ts>
		struct __view_closure;

		template <std::size_t... Is, class Fn, class... Ts>
		struct __view_closure<std::index_sequence<Is...>, Fn, Ts...>
		: private __box<Is, Ts>... {
		private:
			using __box<Is, Ts>::get...;
		public:
			__view_closure() = default;
			constexpr explicit __view_closure(Fn, Ts&&... ts)
			: __box<Is, Ts>{std::forward<Ts>(ts)}... {}

			template <InputRange Rng>
			requires (std::is_lvalue_reference_v<Rng> || View<__f<Rng>>) &&
				Invocable<Fn, Rng, Ts...> && View<std::invoke_result_t<Fn, Rng, Ts...>>
			constexpr auto operator()(Rng&& rng) &&
			{ return Fn{}(std::forward<Rng>(rng), std::move(*this).template get<Is>()...); }

			template <InputRange Rng>
			requires (std::is_lvalue_reference_v<Rng> || View<__f<Rng>>) &&
				Invocable<Fn, Rng, Ts &...> && View<std::invoke_result_t<Fn, Rng, Ts &...>>
			constexpr auto operator()(Rng&& rng) &
			{ return Fn{}(std::forward<Rng>(rng), this->template get<Is>()...); }

			template <InputRange Rng>
			requires (std::is_lvalue_reference_v<Rng> || View<__f<Rng>>) &&
				Invocable<Fn, Rng, const Ts &...> && View<std::invoke_result_t<Fn, Rng, const Ts &...>>
			constexpr auto operator()(Rng&& rng) const &
			{ return Fn{}(std::forward<Rng>(rng), this->template get<Is>()...); }
		};

		template <Semiregular Fn, CopyConstructible... Ts>
		struct view_closure
		: __pipeable<view_closure<Fn, Ts...>>
		, __view_closure<std::make_index_sequence<sizeof...(Ts)>, Fn, Ts...> {
			view_closure() = default;
			using __view_closure<std::make_index_sequence<sizeof...(Ts)>, Fn, Ts...>::__view_closure;
		};

		template <Semiregular Fn, CopyConstructible... Ts>
		view_closure(Fn, Ts&&...) -> view_closure<Fn, Ts...>;

		template <Pipeable A, Pipeable B>
		struct __view_pipeline : private __pipeable<__view_pipeline<A, B>> {
		private:
			A left_;
			B right_;
		public:
			__view_pipeline() = default;
			constexpr __view_pipeline(A&& left, B&& right)
			: left_(std::move(left)), right_(std::move(right)) {}

			template <Range R>
			requires (std::is_lvalue_reference_v<R> || View<__f<R>>) &&
				Invocable<A&&, R> &&
				Invocable<B&&, std::invoke_result_t<A&&, R>>
			constexpr decltype(auto) operator()(R&& r) &&
			{ return std::move(right_)(std::move(left_)(std::forward<R>(r))); }

			template <Range R>
			requires (std::is_lvalue_reference_v<R> || View<__f<R>>) &&
				Invocable<A&, R> &&
				Invocable<B&, std::invoke_result_t<A&, R>>
			constexpr decltype(auto) operator()(R&& r) &
			{ return right_(left_(std::forward<R>(r))); }

			template <Range R>
			requires (std::is_lvalue_reference_v<R> || View<__f<R>>) &&
				Invocable<const A&, R> &&
				Invocable<const B&, std::invoke_result_t<const A&, R>>
			constexpr decltype(auto) operator()(R&& r) const &
			{ return right_(left_(std::forward<R>(r))); }
		};

		template <Pipeable A, Pipeable B>
		__view_pipeline(A, B) -> __view_pipeline<A, B>;

		template <Pipeable A, Pipeable B>
		constexpr auto operator|(A left, B right)
		{ return __view_pipeline{std::move(left), std::move(right)}; }
	}
} STL2_CLOSE_NAMESPACE

#endif
