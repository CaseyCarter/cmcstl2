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
#ifndef STL2_DETAIL_IN_PLACE_HPP
#define STL2_DETAIL_IN_PLACE_HPP

#include <exception>
#include <meta/meta.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	struct in_place_tag { in_place_tag() = delete; };
	using in_place_t = in_place_tag(&)(in_place_tag);
	inline in_place_tag in_place(in_place_tag)
	{ std::terminate(); }

	template <class T>
	using in_place_type_t = in_place_tag(&)(in_place_tag, meta::id<T>);
	template <class T>
	inline in_place_tag in_place(in_place_tag, meta::id<T>)
	{ std::terminate(); }

	template <std::size_t I>
	using in_place_index_t = in_place_tag(&)(in_place_tag, meta::size_t<I>);
	template <std::size_t I>
	inline in_place_tag in_place(in_place_tag, meta::size_t<I>)
	{ std::terminate(); }
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_IN_PLACE_HPP
