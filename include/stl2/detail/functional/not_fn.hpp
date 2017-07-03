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
	namespace detail {
		template<class F, class... Args>
		concept bool NegateInvocable = Invocable<F, Args...> &&
			requires(F&& f, Args&&... args) {
				{ !__invoke::impl(static_cast<F&&>(f), static_cast<Args&&>(args)...) } -> Boolean;
			};
	} // namespace detail

	template <ext::MoveConstructibleObject F>
	struct __not_fn : private detail::ebo_box<F, __not_fn<F>> {
	private:
		using box_t = detail::ebo_box<F, __not_fn<F>>;
	public:
		template <class FF>
		requires Constructible<F, FF>
		explicit constexpr __not_fn(FF&& arg)
		noexcept(std::is_nothrow_constructible<F, FF>::value)
		: box_t(static_cast<FF&&>(arg))
		{}
		template <class... Args>
		requires detail::NegateInvocable<F&, Args...>
		constexpr bool operator()(Args&&... args) &
		noexcept(noexcept(!__invoke::impl(std::declval<F&>(), static_cast<Args&&>(args)...)))
		{
			return !__invoke::impl(box_t::get(), static_cast<Args&&>(args)...);
		}
		template <class... Args>
		requires detail::NegateInvocable<const F&, Args...>
		constexpr bool operator()(Args&&... args) const &
		noexcept(noexcept(!__invoke::impl(std::declval<const F&>(), static_cast<Args&&>(args)...)))
		{
			return !__invoke::impl(box_t::get(), static_cast<Args&&>(args)...);
		}
		template <class... Args>
		requires detail::NegateInvocable<F, Args...>
		constexpr bool operator()(Args&&... args) &&
		noexcept(noexcept(!__invoke::impl(std::declval<F>(), static_cast<Args&&>(args)...)))
		{
			return !__invoke::impl(std::move(box_t::get()), static_cast<Args&&>(args)...);
		}
		template <class... Args>
		requires detail::NegateInvocable<const F, Args...>
		constexpr bool operator()(Args&&... args) const &&
		noexcept(noexcept(!__invoke::impl(std::declval<const F>(), static_cast<Args&&>(args)...)))
		{
			return !__invoke::impl(std::move(box_t::get()), static_cast<Args&&>(args)...);
		}
	};

	template <class F>
	requires MoveConstructible<__f<F>>
	STL2_CONSTEXPR_EXT __not_fn<__f<F>> not_fn(F&& f)
	STL2_NOEXCEPT_RETURN(
		__not_fn<__f<F>>{static_cast<F&&>(f)}
	)
} STL2_CLOSE_NAMESPACE

#endif
