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
#ifndef STL2_DETAIL_FUNCTIONAL_INVOKE_HPP
#define STL2_DETAIL_FUNCTIONAL_INVOKE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// invoke [C++ WP]
	//
	namespace __invoke {
		template <class>
		constexpr bool is_reference_wrapper = false;
		template <class T>
		constexpr bool is_reference_wrapper<std::reference_wrapper<T>> = true;

		template <class, class T1>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			*std::forward<T1>(t1)
		)

		template <class T, class T1>
		requires
			DerivedFrom<std::decay_t<T1>, T>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			std::forward<T1>(t1)
		)

		template <class, class T1>
		requires
			is_reference_wrapper<std::decay_t<T1>>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			std::forward<T1>(t1).get()
		)

		template <_Is<std::is_function> F, class T, class T1, class... Args>
		constexpr decltype(auto) impl(F (T::*f), T1&& t1, Args&&... args) = delete;

		template <_Is<std::is_function> F, class T, class T1, class... Args>
		requires
			requires(F (T::*f), T1&& t1, Args&&... args) {
				(coerce<T>(std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
			}
		constexpr decltype(auto) impl(F (T::*f), T1&& t1, Args&&... args)
		STL2_NOEXCEPT_RETURN(
			(coerce<T>(std::forward<T1>(t1)).*f)(std::forward<Args>(args)...)
		)

		template <ext::Object D, class T, class T1>
		constexpr decltype(auto) impl(D (T::*f), T1&& t1) = delete;

		template <ext::Object D, class T, class T1>
		requires
			requires(D (T::*f), T1&& t1) {
				coerce<T>(std::forward<T1>(t1)).*f;
			}
		constexpr decltype(auto) impl(D (T::*f), T1&& t1)
		STL2_NOEXCEPT_RETURN(
			(coerce<T>(std::forward<T1>(t1)).*f)
		)

		template <class F, class... Args>
		requires
			requires(F&& f, Args&&... args) {
				std::forward<F>(f)(std::forward<Args>(args)...);
			}
		constexpr decltype(auto) impl(F&& f, Args&&... args)
		STL2_NOEXCEPT_RETURN(
			std::forward<F>(f)(std::forward<Args>(args)...)
		)
	}
	template <class F, class... Args>
	requires
		requires(F&& f, Args&&... args) {
			__invoke::impl(std::forward<F>(f), std::forward<Args>(args)...);
		}
	STL2_CONSTEXPR_EXT decltype(auto) invoke(F&& f, Args&&... args)
	STL2_NOEXCEPT_RETURN(
		__invoke::impl(std::forward<F>(f), std::forward<Args>(args)...)
	)

	template<class> struct result_of {};
	template<class R, class... Args>
	requires requires { __stl2::invoke(std::declval<R>(), std::declval<Args>()...); }
	struct result_of<R(Args...)> {
		using type = decltype(__stl2::invoke(std::declval<R>(), std::declval<Args>()...));
	};
	template<class T>
	using result_of_t = meta::_t<result_of<T>>;
} STL2_CLOSE_NAMESPACE

#endif
