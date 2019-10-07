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

#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// not_fn from C++17
	//
	template<ext::move_constructible_object F>
	struct __not_fn : private detail::ebo_box<F, __not_fn<F>> {
	private:
		using box_t = detail::ebo_box<F, __not_fn<F>>;
	public:
		template<class FF>
		requires constructible_from<F, FF>
		explicit constexpr __not_fn(FF&& arg)
		noexcept(std::is_nothrow_constructible_v<F, FF>)
		: box_t(static_cast<FF&&>(arg))
		{}
		template<class... Args>
		requires predicate<F&, Args...>
		constexpr bool operator()(Args&&... args) &
		noexcept(noexcept(!bool(__stl2::invoke(std::declval<F&>(), static_cast<Args&&>(args)...))))
		{
			return !bool(__stl2::invoke(box_t::get(), static_cast<Args&&>(args)...));
		}
		template<class... Args>
		requires predicate<const F&, Args...>
		constexpr bool operator()(Args&&... args) const &
		noexcept(noexcept(!bool(__stl2::invoke(std::declval<const F&>(), static_cast<Args&&>(args)...))))
		{
			return !bool(__stl2::invoke(box_t::get(), static_cast<Args&&>(args)...));
		}
		template<class... Args>
		requires predicate<F, Args...>
		constexpr bool operator()(Args&&... args) &&
		noexcept(noexcept(!bool(__stl2::invoke(std::declval<F>(), static_cast<Args&&>(args)...))))
		{
			return !bool(__stl2::invoke(std::move(box_t::get()), static_cast<Args&&>(args)...));
		}
		template<class... Args>
		requires predicate<const F, Args...>
		constexpr bool operator()(Args&&... args) const &&
		noexcept(noexcept(!bool(__stl2::invoke(std::declval<const F>(), static_cast<Args&&>(args)...))))
		{
			return !bool(__stl2::invoke(std::move(box_t::get()), static_cast<Args&&>(args)...));
		}
	};

	template<class F>
	requires move_constructible<__f<F>>
	constexpr __not_fn<__f<F>> not_fn(F&& f)
	STL2_NOEXCEPT_RETURN(
		__not_fn<__f<F>>{static_cast<F&&>(f)}
	)
} STL2_CLOSE_NAMESPACE

#endif
