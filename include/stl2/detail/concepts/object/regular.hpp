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
#ifndef STL2_DETAIL_CONCEPTS_REGULAR_HPP
#define STL2_DETAIL_CONCEPTS_REGULAR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/object/semiregular.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Regular [concepts.lib.object.regular]
	//
	template<class T>
	STL2_CONCEPT Regular = Semiregular<T> && EqualityComparable<T>;
} STL2_CLOSE_NAMESPACE

#endif
