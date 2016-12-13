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
#ifndef STL2_DETAIL_UTILITY_IN_PLACE_HPP
#define STL2_DETAIL_UTILITY_IN_PLACE_HPP

#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	struct in_place_t {
		explicit in_place_t() = default;
	};
	inline namespace {
		constexpr auto& in_place = detail::static_const<in_place_t>::value;
	}

	template <class>
	struct in_place_type_t {
		explicit in_place_type_t() = default;
	};
	inline namespace {
		template <class T>
		constexpr const in_place_type_t<T>& in_place_type = detail::static_const<in_place_type_t<T>>::value;
	}

	template <std::size_t>
	struct in_place_index_t {
		explicit in_place_index_t() = default;
	};
	inline namespace {
		template <std::size_t I>
		constexpr const in_place_index_t<I>& in_place_index = detail::static_const<in_place_index_t<I>>::value;
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_UTILITY_IN_PLACE_HPP
