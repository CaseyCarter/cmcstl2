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
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		struct __pipeable_base;

		template<class T>
		META_CONCEPT Pipeable =
			derived_from<T, __pipeable_base> && ext::CopyConstructibleObject<T>;

		template<Pipeable, Pipeable>
		struct __view_pipeline;

		struct __pipeable_base {
			template<Pipeable A, Pipeable B>
			requires constructible_from<__uncvref<A>, A> &&
				constructible_from<__uncvref<B>, B>
			friend constexpr auto operator|(A&& left, B&& right)
			{ return __view_pipeline{static_cast<A&&>(left), static_cast<B&&>(right)}; }
		};

		template<class Derived>
		struct __pipeable : __pipeable_base {
			template<class Rng>
			friend constexpr auto operator|(Rng&& rng, Derived& c)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(c(std::forward<Rng>(rng)))
			{ return c(std::forward<Rng>(rng)); }

			template<class Rng>
			friend constexpr auto operator|(Rng&& rng, const Derived& c)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(c(std::forward<Rng>(rng)))
			{ return c(std::forward<Rng>(rng)); }

			template<class Rng>
			friend constexpr auto operator|(Rng&& rng, Derived&& c)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(std::move(c)(std::forward<Rng>(rng)))
			{ return std::move(c)(std::forward<Rng>(rng)); }

#if STL2_WORKAROUND_CLANG_40150
			template<class Rng>
			friend constexpr auto operator|(Rng&& rng, const Derived&& c)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(std::move(c)(std::forward<Rng>(rng)))
			{ return std::move(c)(std::forward<Rng>(rng)); }
#else
			template<class Rng>
			friend void operator|(Rng&& rng, const Derived&& c) = delete;
#endif
		};

		template<class Indices, class Fn, class... Ts>
		struct __view_closure;

		template<std::size_t, class T>
		struct __box {
			T value_;
		};

		template<std::size_t... Is, class Fn, class... Ts>
		struct STL2_EMPTY_BASES __view_closure<std::index_sequence<Is...>, Fn, Ts...>
		: private __box<Is, Ts>... {
			__view_closure() = default;
			constexpr explicit __view_closure(Fn, Ts&&... ts)
			: __box<Is, Ts>{std::forward<Ts>(ts)}... {}

			template<input_range Rng>
			requires viewable_range<Rng> && invocable<Fn, Rng, Ts...> &&
				view<invoke_result_t<Fn, Rng, Ts...>>
			constexpr auto operator()(Rng&& rng) && {
				return Fn{}(std::forward<Rng>(rng),
					static_cast<__box<Is, Ts>&&>(*this).value_...);
			}
			template<input_range Rng>
			requires viewable_range<Rng> && invocable<Fn, Rng, Ts &...> &&
				view<invoke_result_t<Fn, Rng, Ts &...>>
			constexpr auto operator()(Rng&& rng) & {
				return Fn{}(std::forward<Rng>(rng),
					static_cast<__box<Is, Ts>&>(*this).value_...);
			}
			template<input_range Rng>
			requires viewable_range<Rng> && invocable<Fn, Rng, const Ts &...> &&
				view<invoke_result_t<Fn, Rng, const Ts &...>>
			constexpr auto operator()(Rng&& rng) const & {
				return Fn{}(std::forward<Rng>(rng),
					static_cast<const __box<Is, Ts>&>(*this).value_...);
			}
			template<input_range Rng>
			requires viewable_range<Rng> && invocable<Fn, Rng, const Ts &...> &&
				view<invoke_result_t<Fn, Rng, const Ts &...>>
			void operator()(Rng&& rng) const && = delete;
		};

		template<semiregular Fn, copy_constructible... Ts>
		struct STL2_EMPTY_BASES view_closure
		: __pipeable<view_closure<Fn, Ts...>>
		, __view_closure<std::index_sequence_for<Ts...>, Fn, Ts...> {
			using __view_closure<std::index_sequence_for<Ts...>, Fn, Ts...>::__view_closure;
		};

		template<semiregular Fn, copy_constructible... Ts>
		view_closure(Fn, Ts&&...) -> view_closure<Fn, Ts...>;

		template<Pipeable A, Pipeable B>
		struct __view_pipeline : __pipeable<__view_pipeline<A, B>> {
		private:
			A left_;
			B right_;
		public:
			__view_pipeline() = default;
			constexpr __view_pipeline(A&& left, B&& right)
			: left_(std::move(left)), right_(std::move(right)) {}

			template<viewable_range R>
			requires invocable<A, R> && invocable<B, invoke_result_t<A, R>>
			constexpr decltype(auto) operator()(R&& r) &&
			{ return std::move(right_)(std::move(left_)(std::forward<R>(r))); }

			template<viewable_range R>
			requires invocable<A&, R> && invocable<B&, invoke_result_t<A&, R>>
			constexpr decltype(auto) operator()(R&& r) &
			{ return right_(left_(std::forward<R>(r))); }

			template<viewable_range R>
			requires invocable<const A&, R> &&
				invocable<const B&, invoke_result_t<const A&, R>>
			constexpr decltype(auto) operator()(R&& r) const &
			{ return right_(left_(std::forward<R>(r))); }
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
