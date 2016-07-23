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
#ifndef STL2_DETAIL_ITERATOR_UNREACHABLE_HPP
#define STL2_DETAIL_ITERATOR_UNREACHABLE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// unreachable [unreachable.sentinel]
//
STL2_OPEN_NAMESPACE {
	class unreachable {};

	constexpr bool operator==(const auto&, unreachable) noexcept {
		return false;
	}
	constexpr bool operator==(unreachable, const auto&) noexcept {
		return false;
	}

	constexpr bool operator!=(const auto&, unreachable) noexcept {
		return true;
	}
	constexpr bool operator!=(unreachable, const auto&) noexcept {
		return true;
	}
} STL2_CLOSE_NAMESPACE

#endif
