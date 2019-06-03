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
#ifndef STL2_DETAIL_CLOSURE_HPP
#define STL2_DETAIL_CLOSURE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/callable.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		struct __pipeable_base;

		template<class T>
		META_CONCEPT Pipeable =
			DerivedFrom<T, __pipeable_base> && ext::CopyConstructibleObject<T>;

		template<Pipeable, Pipeable>
		struct __pipeline;

		struct __pipeable_base {
			template<Pipeable Left, Pipeable Right>
			requires Constructible<__uncvref<Left>, Left> &&
				Constructible<__uncvref<Right>, Right>
			friend constexpr auto operator|(Left&& left, Right&& right)
			{ return __pipeline{static_cast<Left&&>(left), static_cast<Right&&>(right)}; }
		};

		template<class Derived>
		struct __pipeable : __pipeable_base {
			template<class T>
			friend constexpr auto operator|(T&& t, Derived& d)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(d(static_cast<T&&>(t)))
			{ return d(static_cast<T&&>(t)); }

			template<class T>
			friend constexpr auto operator|(T&& t, const Derived& d)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(d(static_cast<T&&>(t)))
			{ return d(static_cast<T&&>(t)); }

			template<class T>
			friend constexpr auto operator|(T&& t, Derived&& d)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(static_cast<Derived&&>(d)(static_cast<T&&>(t)))
			{ return static_cast<Derived&&>(d)(static_cast<T&&>(t)); }

			template<class T>
			friend constexpr auto operator|(T&& t, const Derived&& d)
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68093
				-> decltype(static_cast<const Derived&&>(d)(static_cast<T&&>(t)))
			{ return static_cast<const Derived&&>(d)(static_cast<T&&>(t)); }
		};

		template<class _Ty>
		META_CONCEPT _Stateless = Constructible<_Ty> && std::is_empty_v<_Ty>;

		template<std::size_t, class T>
		struct __box {
			STL2_NO_UNIQUE_ADDRESS T value_;
		};

		template<class Indices, class Fn, class... Ts>
		struct __closure;

		template<std::size_t... Is, class Fn, class... Ts>
		requires _Stateless<Fn> && (CopyConstructible<Ts> && ...)
		struct STL2_EMPTY_BASES __closure<std::index_sequence<Is...>, Fn, Ts...>
		: private __box<Is, Ts>... {
			__closure() = default;
			constexpr explicit __closure(Fn, Ts&&... ts)
			: __box<Is, Ts>{static_cast<Ts&&>(ts)}... {}

			template<class T>
			requires Invocable<Fn, T, Ts &...>
			constexpr auto operator()(T&& t) & {
				return Fn{}(static_cast<T&&>(t),
					static_cast<__box<Is, Ts>&>(*this).value_...);
			}
			template<class T>
			requires Invocable<Fn, T, const Ts &...>
			constexpr auto operator()(T&& t) const & {
				return Fn{}(static_cast<T&&>(t),
					static_cast<const __box<Is, Ts>&>(*this).value_...);
			}
			template<class T>
			requires Invocable<Fn, T, Ts...>
			constexpr auto operator()(T&& t) && {
				return Fn{}(static_cast<T&&>(t),
					static_cast<__box<Is, Ts>&&>(*this).value_...);
			}
			template<class T>
			requires Invocable<Fn, T, const Ts...>
			constexpr auto operator()(T&& t) const && {
				return Fn{}(static_cast<T&&>(t),
					static_cast<const __box<Is, Ts>&&>(*this).value_...);
			}
		};

		template<_Stateless Fn, CopyConstructible... Ts>
		struct STL2_EMPTY_BASES closure
		: __pipeable<closure<Fn, Ts...>>
		, __closure<std::index_sequence_for<Ts...>, Fn, Ts...> {
			using __closure<std::index_sequence_for<Ts...>, Fn, Ts...>::__closure;
		};

		template<class Fn, class... Ts>
		closure(Fn, Ts&&...) -> closure<Fn, Ts...>;

		template<Pipeable Left, Pipeable Right>
		struct __pipeline : __pipeable<__pipeline<Left, Right>> {
			__pipeline() = default;
			constexpr __pipeline(Left&& left, Right&& right)
			: left_(std::move(left)), right_(std::move(right)) {}

			template<class T>
			requires Invocable<Left&, T> && Invocable<Right&, invoke_result_t<Left&, T>>
			constexpr decltype(auto) operator()(T&& t) &
			{ return right_(left_(static_cast<T&&>(t))); }

			template<class T>
			requires Invocable<const Left&, T> &&
				Invocable<const Right&, invoke_result_t<const Left&, T>>
			constexpr decltype(auto) operator()(T&& t) const &
			{ return right_(left_(static_cast<T&&>(t))); }

			template<class T>
			requires Invocable<Left, T> && Invocable<Right, invoke_result_t<Left, T>>
			constexpr decltype(auto) operator()(T&& t) &&
			{ return static_cast<Right&&>(right_)(static_cast<Left&&>(left_)(static_cast<T&&>(t))); }
		private:
			STL2_NO_UNIQUE_ADDRESS Left left_;
			STL2_NO_UNIQUE_ADDRESS Right right_;
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
