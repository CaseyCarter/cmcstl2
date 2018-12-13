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
	template<__can_reference> struct reference_wrapper;

	///////////////////////////////////////////////////////////////////////////
	// invoke [C++ WP]
	//
	namespace __invoke {
		template<class, class T1>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			*static_cast<T1&&>(t1)
		)

		template<class T, class T1>
		requires DerivedFrom<std::decay_t<T1>, T>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			static_cast<T1&&>(t1)
		)

		template<class T>
		inline constexpr bool is_reference_wrapper =
			meta::is<T, reference_wrapper>::value ||
			meta::is<T, std::reference_wrapper>::value;

		template<class, class T1>
		requires is_reference_wrapper<std::decay_t<T1>>
		constexpr decltype(auto) coerce(T1&& t1)
		STL2_NOEXCEPT_RETURN(
			static_cast<T1&&>(t1).get()
		)
	}

	template<_Is<std::is_function> F, class T, class T1, class... Args>
	constexpr decltype(auto) invoke(F (T::*f), T1&& t1, Args&&... args)
	STL2_NOEXCEPT_REQUIRES_RETURN(
		(__invoke::coerce<T>(static_cast<T1&&>(t1)).*f)(static_cast<Args&&>(args)...)
	)

	template<ext::Object D, class T, class T1>
	constexpr decltype(auto) invoke(D (T::*f), T1&& t1)
	STL2_NOEXCEPT_REQUIRES_RETURN(
		__invoke::coerce<T>(static_cast<T1&&>(t1)).*f
	)

	template<class F, class... Args>
	constexpr decltype(auto) invoke(F&& f, Args&&... args)
	STL2_NOEXCEPT_REQUIRES_RETURN(
		static_cast<F&&>(f)(static_cast<Args&&>(args)...)
	)

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
		T& t_;

		static constexpr T& fun(T& t) noexcept { return t; }
		static constexpr void fun(T&& t) = delete;
	public:
		using type = T;

		template<_NotSameAs<reference_wrapper> U>
		constexpr reference_wrapper(U&& u)
		noexcept(noexcept(fun(static_cast<U&&>(u))))
		requires requires { fun(static_cast<U&&>(u)); }
		: t_(fun(static_cast<U&&>(u))) {}

		constexpr operator T&() const noexcept { return t_; }
		constexpr T& get() const noexcept { return t_; }

		template<class... Args>
		requires requires {
			__stl2::invoke(std::declval<T&>(), std::declval<Args&&>()...);
		}
		constexpr decltype(auto) operator()(Args&&... args) const
		noexcept(noexcept(__stl2::invoke(
			std::declval<T&>(), static_cast<Args&&>(args)...))) {
			return __stl2::invoke(t_, static_cast<Args&&>(args)...);
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
