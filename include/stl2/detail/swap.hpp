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
#ifndef STL2_DETAIL_SWAP_HPP
#define STL2_DETAIL_SWAP_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object/assignable.hpp>
#include <stl2/detail/concepts/object/move_constructible.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// exchange [utility.exchange]
	// Not to spec: constexpr
	// See https://github.com/ericniebler/stl2/322
	template <class T, class U = T>
	requires
		MoveConstructible<T> && Assignable<T&, U>
	constexpr T exchange(T& t, U&& u)
	noexcept(is_nothrow_move_constructible<T>::value &&
		is_nothrow_assignable<T&, U>::value)
	{
		T tmp(std::move(t));
		t = std::forward<U>(u);
		return tmp;
	}

	///////////////////////////////////////////////////////////////////////////
	// swap [utility.swap]
	//
	namespace __swap {
		// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2152
		// http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#2171

		// Poison pill for std::swap. If ADL for "swap" finds std::swap and
		// unqualified name lookup finds this overload, the ambiguity causes
		// overload resolution to fail. If ADL doesn't find any declarations
		// of "swap", this deleted overload directly prevents unqualified
		// name lookup from searching further up the namespace hierarchy and
		// finding std::swap. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template <class T> void swap(T&, T&) = delete;
		template <class T, std::size_t N> void swap(T(&)[N], T(&)[N]) = delete;

		template <class T, class U>
		constexpr bool has_customization = false;
		template <class T, class U>
		requires
			requires(T&& t, U&& u) { swap((T&&)t, (U&&)u); }
		constexpr bool has_customization<T, U> = true;

		template <class F, class T, class U>
		constexpr bool has_operator = false;
		template <class F, class T, class U>
		requires
			requires(const F& f, T& t, U& u) { f(t, u); }
		constexpr bool has_operator<F, T, U> = true;

		class fn {
		public:
			template <class T, class U>
			requires has_customization<T, U>
			constexpr void operator()(T&& t, U&& u) const
			STL2_NOEXCEPT_RETURN(
				(void)swap(std::forward<T>(t), std::forward<U>(u))
			)
			template <class T>
			requires
				!has_customization<T&, T&> && MoveConstructible<T> &&
				Assignable<T&, T&&>
			constexpr void operator()(T& a, T& b) const
			STL2_NOEXCEPT_RETURN(
				(void)(b = __stl2::exchange(a, std::move(b)))
			)
			template <class T, class U, std::size_t N, class F = fn>
			requires
				!has_customization<T(&)[N], U(&)[N]> && has_operator<F, T, U>
			constexpr void operator()(T (&t)[N], U (&u)[N]) const
			noexcept(noexcept(declval<const F&>()(t[0], u[0])))
			{
				for (std::size_t i = 0; i < N; ++i) {
					(*this)(t[i], u[i]);
				}
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& swap = detail::static_const<__swap::fn>::value;
	}

	///////////////////////////////////////////////////////////////////////////
	// Swappable [concepts.lib.corelang.swappable]
	//
	template <class T>
	concept bool Swappable =
		requires(T& a, T& b) {
			__stl2::swap(a, b);
		};

	template <class T, class U>
	concept bool SwappableWith =
		CommonReference<
			const remove_reference_t<T>&,
			const remove_reference_t<U>&> &&
		requires(T&& t, U&&u) {
			__stl2::swap((T&&)t, (T&&)t);
			__stl2::swap((U&&)u, (U&&)u);
			__stl2::swap((T&&)t, (U&&)u);
			__stl2::swap((U&&)u, (T&&)t);
		};

	namespace models {
		template <class T>
		constexpr bool Swappable = false;
		__stl2::Swappable{T}
		constexpr bool Swappable<T> = true;
		template <class T, class U>
		constexpr bool SwappableWith = false;
		__stl2::SwappableWith{T, U}
		constexpr bool SwappableWith<T, U> = true;
	}

	template <class T, class U>
	constexpr bool is_nothrow_swappable_v = false;

	SwappableWith{T, U}
	constexpr bool is_nothrow_swappable_v<T, U> =
		noexcept(__stl2::swap(std::declval<T>(), std::declval<U>())) &&
		noexcept(__stl2::swap(std::declval<U>(), std::declval<T>()));

	template <class T, class U>
	using is_nothrow_swappable_t =
		meta::bool_<is_nothrow_swappable_v<T, U>>;

	template <class T, class U>
	struct is_nothrow_swappable :
		is_nothrow_swappable_t<T, U> {};
} STL2_CLOSE_NAMESPACE

#endif
