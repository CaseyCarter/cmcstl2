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
#ifndef STL2_UTILITY_HPP
#define STL2_UTILITY_HPP

#include <utility>

#include <stl2/functional.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/tuple_like.hpp>

STL2_OPEN_NAMESPACE {
	////////////////////////////////////////////////////////////////////////////
	// tagged_pair
	// Not to spec: Kill it with fire.
	//
	template<TaggedType F, TaggedType S>
	using tagged_pair =
		tagged<pair<__tagged::element<F>, __tagged::element<S>>,
			__tagged::specifier<F>, __tagged::specifier<S>>;

	////////////////////////////////////////////////////////////////////////////
	// make_tagged_pair
	// Not to spec: Kill it with fire.
	//
	template<TagSpecifier Tag1, TagSpecifier Tag2, class T1, class T2>
	requires
		requires {
			typename tagged_pair<Tag1(__unwrap<T1>), Tag2(__unwrap<T2>)>;
		}
	constexpr auto make_tagged_pair(T1&& x, T2&& y) {
		return tagged_pair<Tag1(__unwrap<T1>), Tag2(__unwrap<T2>)>{
			std::forward<T1>(x), std::forward<T2>(y)
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
