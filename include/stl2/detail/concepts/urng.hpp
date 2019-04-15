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
#ifndef STL2_DETAIL_CONCEPTS_URNG_HPP
#define STL2_DETAIL_CONCEPTS_URNG_HPP

#include <stl2/detail/concepts/callable.hpp>

STL2_OPEN_NAMESPACE {
	template<auto> struct __require_constant; // not defined

	template<class G>
	META_CONCEPT UniformRandomBitGenerator =
		Invocable<G&> && UnsignedIntegral<invoke_result_t<G&>> &&
		requires {
#ifdef META_HAS_P1084
			{ G::min() } -> Same<invoke_result_t<G&>>;
			{ G::max() } -> Same<invoke_result_t<G&>>;
#else
			G::min(); requires Same<decltype(G::min()), invoke_result_t<G&>>;
			G::max(); requires Same<decltype(G::max()), invoke_result_t<G&>>;
#endif
#if 1 // This is the PR for https://wg21.link/lwg3150
			typename __require_constant<G::min()>;
			typename __require_constant<G::min()>;
			requires G::min() < G::max();
#endif
		};
} STL2_CLOSE_NAMESPACE

#endif
