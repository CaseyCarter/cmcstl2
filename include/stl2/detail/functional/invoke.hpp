// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
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
		STL2_NOEXCEPT_REQUIRES_RETURN(
			*static_cast<T1&&>(t1)
		)

		template<class T, class T1>
		requires std::is_base_of_v<T, __uncvref<T1>>
		constexpr auto&& coerce(T1&& t1) noexcept {
			return static_cast<T1&&>(t1);
		}

		template<class T>
		META_CONCEPT is_reference_wrapper =
			meta::is_v<T, reference_wrapper> ||
			meta::is_v<T, std::reference_wrapper>;

		template<class, class RW>
		requires is_reference_wrapper<RW>
		constexpr auto& coerce(RW rw) noexcept {
			return rw.get();
		}
	}

	template<class F, class T, class T1, class... Args>
	constexpr decltype(auto) invoke(F (T::*pmf), T1&& t1, Args&&... args)
	STL2_NOEXCEPT_REQUIRES_RETURN(
		(__invoke::coerce<T>(static_cast<T1&&>(t1)).*pmf)(static_cast<Args&&>(args)...)
	)

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 10
	template<ext::object D, class T, class T1>
#else
	template<class D, class T, class T1>
#endif
	constexpr decltype(auto) invoke(D (T::*pmd), T1&& t1)
	STL2_NOEXCEPT_REQUIRES_RETURN(
		__invoke::coerce<T>(static_cast<T1&&>(t1)).*pmd
	)

	template<class F, class... Args>
	constexpr decltype(auto) invoke(F&& f, Args&&... args)
	STL2_NOEXCEPT_REQUIRES_RETURN(
		static_cast<F&&>(f)(static_cast<Args&&>(args)...)
	)

	template<class F, class... Args>
	using invoke_result_t = decltype(__stl2::invoke(std::declval<F>(), std::declval<Args>()...));
	template<class, class...> struct invoke_result {};
	template<class F, class... Args>
	requires requires { typename invoke_result_t<F, Args...>; }
	struct invoke_result<F, Args...> {
		using type = invoke_result_t<F, Args...>;
	};

	template<__can_reference T>
	struct reference_wrapper {
	private:
		T* t_;

		static constexpr T& fun(T& t) noexcept { return t; }
		static constexpr void fun(T&& t) = delete;
	public:
		using type = T;

		template<class U>
		requires (!derived_from<U, reference_wrapper>)
		constexpr reference_wrapper(U&& u)
		noexcept(noexcept(fun(static_cast<U&&>(u))))
		requires requires { fun(static_cast<U&&>(u)); }
		: t_(std::addressof(fun(static_cast<U&&>(u)))) {}

		constexpr operator T&() const noexcept { return *t_; }
		constexpr T& get() const noexcept { return *t_; }

		template<class... Args>
		requires requires(T& t, Args&&... args) { __stl2::invoke(t, static_cast<Args&&>(args)...); }
		constexpr decltype(auto) operator()(Args&&... args) const
		noexcept(noexcept(__stl2::invoke(*t_, static_cast<Args&&>(args)...))) {
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
