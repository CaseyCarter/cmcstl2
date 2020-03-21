// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONCEPTS_NUMERIC_MONOID_HPP
#define STL2_DETAIL_CONCEPTS_NUMERIC_MONOID_HPP

#include <stl2/detail/concepts/numeric/semigroup.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/numeric_traits/identity.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class BOp, class T, class U>
		META_CONCEPT Monoid = Semigroup<BOp, T, U> && requires {
			typename two_sided_identity_t<BOp, __uncvref<T>, __uncvref<U>>;
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_CONCEPTS_NUMERIC_MONOID_HPP
