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

////////////////////////////////////////////////////////////////////////////////
// invoke(_result(_t)?)?, reference_wrapper, and c?ref all properly constexpr
// per the proposed design in P1065.
//
STL2_OPEN_NAMESPACE {
	template<__can_reference> struct reference_wrapper;

	namespace __invoke {
		template<class, class T1>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			*static_cast<T1&&>(t1)
		)

		template<class T, class T1>
		requires derived_from<std::decay_t<T1>, T>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			static_cast<T1&&>(t1)
		)

		template<class T>
		inline constexpr bool is_reference_wrapper =
			meta::is_v<T, reference_wrapper> ||
			meta::is_v<T, std::reference_wrapper>;

		template<class, class T1>
		requires is_reference_wrapper<std::decay_t<T1>>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			static_cast<T1&&>(t1).get()
		)
	}

	template<class F, class T, class T1, class... Args>
	requires std::is_function_v<F>
	constexpr decltype(auto) invoke(F (T::*f), T1&& t1, Args&&... args)
#if STL2_WORKAROUND_CLANGC_50
	noexcept(noexcept((__invoke::coerce<T>(static_cast<T1&&>(t1)).*f)(
		static_cast<Args&&>(args)...)))
	requires requires(F (T::*f), T1&& t1, Args&&... args) {
		(__invoke::coerce<T>(static_cast<T1&&>(t1)).*f)(static_cast<Args&&>(args)...);
	} {
		return (__invoke::coerce<T>(static_cast<T1&&>(t1)).*f)(
			static_cast<Args&&>(args)...);
	}
#else // ^^^ workaround / no workaround vvv
	STL2_NOEXCEPT_REQUIRES_RETURN(
		(__invoke::coerce<T>(static_cast<T1&&>(t1)).*f)(static_cast<Args&&>(args)...)
	)
#endif // STL2_WORKAROUND_CLANGC_50

	template<ext::Object D, class T, class T1>
	constexpr decltype(auto) invoke(D (T::*f), T1&& t1)
#if STL2_WORKAROUND_CLANGC_50
	noexcept(noexcept(__invoke::coerce<T>(static_cast<T1&&>(t1)).*f))
	requires requires(D (T::*f), T1&& t1) {
		__invoke::coerce<T>(static_cast<T1&&>(t1)).*f;
	} {
		return __invoke::coerce<T>(static_cast<T1&&>(t1)).*f;
	}
#else // ^^^ workaround / no workaround vvv
	STL2_NOEXCEPT_REQUIRES_RETURN(
		__invoke::coerce<T>(static_cast<T1&&>(t1)).*f
	)
#endif // STL2_WORKAROUND_CLANGC_50

	template<class F, class... Args>
	constexpr decltype(auto) invoke(F&& f, Args&&... args)
#if STL2_WORKAROUND_CLANGC_50
	noexcept(noexcept(static_cast<F&&>(f)(static_cast<Args&&>(args)...)))
	requires requires(F&& f, Args&&... args) {
		static_cast<F&&>(f)(static_cast<Args&&>(args)...);
	} {
		return static_cast<F&&>(f)(static_cast<Args&&>(args)...);
	}
#else // ^^^ workaround / no workaround vvv
	STL2_NOEXCEPT_REQUIRES_RETURN(
		static_cast<F&&>(f)(static_cast<Args&&>(args)...)
	)
#endif // STL2_WORKAROUND_CLANGC_50

	template<class, class...> struct invoke_result {};
	template<class F, class... Args>
	requires requires { __stl2::invoke(std::declval<F>(), std::declval<Args>()...); }
	struct invoke_result<F, Args...> {
		using type = decltype(__stl2::invoke(std::declval<F>(), std::declval<Args>()...));
	};
	template<class F, class... Args>
	using invoke_result_t = meta::_t<invoke_result<F, Args...>>;

	template<__can_reference T>
	struct reference_wrapper {
	private:
		T* t_;

		static constexpr T& fun(T& t) noexcept { return t; }
		static constexpr void fun(T&& t) = delete;
	public:
		using type = T;

#if STL2_WORKAROUND_CLANGC_42
		template<class U>
		requires _NotSameAs<U, reference_wrapper>
#else // ^^^ workaround / no workaround vvv
		template<_NotSameAs<reference_wrapper> U>
#endif // STL2_WORKAROUND_CLANGC_42
		constexpr reference_wrapper(U&& u)
		noexcept(noexcept(fun(static_cast<U&&>(u))))
#if STL2_WORKAROUND_CLANGC_50
		requires requires(U&& u) { fun(static_cast<U&&>(u)); }
#else // ^^^ workaround / no workaround vvv
		requires requires { fun(static_cast<U&&>(u)); }
#endif // STL2_WORKAROUND_CLANGC_50
		: t_(std::addressof(fun(static_cast<U&&>(u)))) {}

		constexpr operator T&() const noexcept { return *t_; }
		constexpr T& get() const noexcept { return *t_; }

		template<class... Args>
		requires requires {
			__stl2::invoke(std::declval<T&>(), std::declval<Args&&>()...);
		}
		constexpr decltype(auto) operator()(Args&&... args) const
		noexcept(noexcept(__stl2::invoke(
			std::declval<T&>(), static_cast<Args&&>(args)...)))
		{
			return __stl2::invoke(*t_, static_cast<Args&&>(args)...);
		}
	};

	template<class T>
	reference_wrapper(T&) -> reference_wrapper<T>;

	template<class T>
	constexpr reference_wrapper<T> ref(reference_wrapper<T> rw) noexcept {
		return rw;
	}
	template<class T>
	constexpr reference_wrapper<T> ref(std::reference_wrapper<T> rw) noexcept {
		return rw.get();
	}
	template<class T>
	constexpr reference_wrapper<T> ref(T& t) noexcept { return {t}; }
	template<class T>
	void ref(const T&&) = delete;

	template<class T>
	constexpr reference_wrapper<const T>
	cref(reference_wrapper<T> rw) noexcept {
		return rw.get();
	}
	template<class T>
	constexpr reference_wrapper<const T>
	cref(std::reference_wrapper<T> rw) noexcept {
		return rw.get();
	}
	template<class T>
	constexpr reference_wrapper<const T> cref(const T& t) noexcept {
		return {t};
	}
	template<class T>
	void cref(const T&&) = delete;
} STL2_CLOSE_NAMESPACE

#endif
