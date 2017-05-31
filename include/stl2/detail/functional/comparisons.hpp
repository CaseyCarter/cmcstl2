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
#ifndef STL2_DETAIL_FUNCTIONAL_COMPARISONS_HPP
#define STL2_DETAIL_FUNCTIONAL_COMPARISONS_HPP

#include <functional>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// equal_to [comparisons]
	//
	template <class T = void>
		requires EqualityComparable<T> || Same<T, void>
	struct equal_to {
		constexpr bool operator()(const T& a, const T& b) const {
			return a == b;
		}
	};

	template <>
	struct equal_to<void> {
		EqualityComparableWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) == std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// not_equal_to
	//
	template <class T = void>
		requires EqualityComparable<T> || Same<T, void>
	struct not_equal_to {
		constexpr bool operator()(const T& a, const T& b) const {
			return a != b;
		}
	};

	template <>
	struct not_equal_to<void> {
		EqualityComparableWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) != std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// greater
	//
	template <class T = void>
		requires StrictTotallyOrdered<T> || Same<T, void>
	struct greater {
		constexpr bool operator()(const T& a, const T& b) const {
			return a > b;
		}
	};

	template <>
	struct greater<void> {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) > std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	template <class T>
	struct greater<T*> : private std::greater<T*> {
		using std::greater<T*>::operator();
	};

	///////////////////////////////////////////////////////////////////////////
	// less
	//
	template <class T = void>
		requires StrictTotallyOrdered<T> || Same<T, void>
	struct less {
		constexpr bool operator()(const T& a, const T& b) const {
			return a < b;
		}
	};

	template <>
	struct less<void> {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) < std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	template <class T>
	struct less<T*> : private std::less<T*> {
		using std::less<T*>::operator();
	};

	///////////////////////////////////////////////////////////////////////////
	// greater_equal
	//
	template <class T = void>
		requires StrictTotallyOrdered<T> || Same<T, void>
	struct greater_equal {
		constexpr bool operator()(const T& a, const T& b) const {
			return a >= b;
		}
	};

	template <>
	struct greater_equal<void> {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) >= std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	template <class T>
	struct greater_equal<T*> : private std::greater_equal<T*> {
		using std::greater_equal<T*>::operator();
	};

	///////////////////////////////////////////////////////////////////////////
	// less_equal
	//
	template <class T = void>
		requires StrictTotallyOrdered<T> || Same<T, void>
	struct less_equal {
		constexpr bool operator()(const T& a, const T& b) const {
			return a <= b;
		}
	};

	template <>
	struct less_equal<void> {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) <= std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	template <class T>
	struct less_equal<T*> : private std::less_equal<T*> {
		using std::less_equal<T*>::operator();
	};
} STL2_CLOSE_NAMESPACE

#endif
