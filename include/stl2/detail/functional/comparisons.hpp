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
	struct equal_to {
		EqualityComparableWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) == std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// not_equal_to
	//
	struct not_equal_to {
		EqualityComparableWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) != std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// greater
	//
	struct greater {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) > std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// less
	//
	struct less {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) < std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// greater_equal
	//
	struct greater_equal {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) >= std::forward<U>(u);
		}

		using is_transparent = true_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// less_equal
	//
	struct less_equal {
		StrictTotallyOrderedWith{T, U}
		constexpr decltype(auto) operator()(T&& t, U&& u) const {
			return std::forward<T>(t) <= std::forward<U>(u);
		}

		using is_transparent = true_type;
	};
} STL2_CLOSE_NAMESPACE

#endif
