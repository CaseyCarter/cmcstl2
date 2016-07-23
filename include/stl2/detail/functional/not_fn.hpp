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
#ifndef STL2_DETAIL_FUNCTIONAL_NOT_FN_HPP
#define STL2_DETAIL_FUNCTIONAL_NOT_FN_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// not_fn from N4336 Library Fundamentals v2
	//
	template <class F>
	class __not_fn {
		F f_;

	public:
		template <class FF>
		requires Constructible<F, FF>()
		explicit constexpr __not_fn(FF&& arg)
		noexcept(is_nothrow_constructible<F, FF>::value)
		: f_(__stl2::forward<decltype(arg)>(arg)) {}

		template <class...Args>
		requires
			requires (F& f, Args&&...args) {
				!__invoke::impl(f, __stl2::forward<Args>(args)...);
			}
		constexpr bool operator()(Args&&...args) &
		STL2_NOEXCEPT_RETURN(
			!__invoke::impl(f_, __stl2::forward<Args>(args)...)
		)

		template <class...Args>
		requires
			requires (const F& f, Args&&...args) {
				!__invoke::impl(f, __stl2::forward<Args>(args)...);
			}
		constexpr bool operator()(Args&&...args) const&
		STL2_NOEXCEPT_RETURN(
			!__invoke::impl(f_, __stl2::forward<Args>(args)...)
		)

		template <class...Args>
		requires
			requires (F&& f, Args&&...args) {
				!__invoke::impl((F&&)f, __stl2::forward<Args>(args)...);
			}
		constexpr bool operator()(Args&&...args) &&
		STL2_NOEXCEPT_RETURN(
			!__invoke::impl(__stl2::move(f_), __stl2::forward<Args>(args)...)
		)

		template <class...Args>
		requires
			requires (F&& f, Args&&...args) {
				!__invoke::impl((F&&)f, __stl2::forward<Args>(args)...);
			}
		constexpr bool operator()(Args&&...args) const&&
		STL2_NOEXCEPT_RETURN(
			!__invoke::impl(__stl2::move(f_), __stl2::forward<Args>(args)...)
		)
	};

	template <class F>
	requires MoveConstructible<__f<F>>()
	STL2_CONSTEXPR_EXT auto not_fn(F&& f)
	STL2_NOEXCEPT_RETURN(
		__not_fn<__f<F>>{__stl2::forward<F>(f)}
	)
} STL2_CLOSE_NAMESPACE

#endif
