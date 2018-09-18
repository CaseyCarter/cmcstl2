// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMORY_ADDRESSOF_HPP
#define STL2_DETAIL_MEMORY_ADDRESSOF_HPP

#include <stl2/detail/fwd.hpp>
#include <memory>

STL2_OPEN_NAMESPACE {
	namespace detail {
#if defined(__cpp_lib_addressof_constexpr) && __cpp_lib_addressof_constexpr >= 201603
		using std::addressof;

#elif STL2_HAS_BUILTIN(addressof)
		template<class T>
		constexpr T* addressof(T& t) noexcept {
			return __builtin_addressof(t);
		}

#else

		template<class>
		constexpr bool __user_defined_addressof = false;
		template<class T>
		requires
			requires(T& t) { t.operator&(); } ||
			requires(T& t) { operator&(t); }
		constexpr bool __user_defined_addressof<T> = true;

		template<class T>
		requires __user_defined_addressof<T>
		T* __addressof(T& t) noexcept {
			return std::addressof(t);
		}

		constexpr auto __addressof(auto& t) noexcept {
			return &t;
		}

		template<class T>
		constexpr T* addressof(T& t) noexcept {
			return detail::__addressof(t);
		}
#endif // __cpp_lib_addressof_constexpr
	} // namespace detail
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_ADDRESSOF_HPP
