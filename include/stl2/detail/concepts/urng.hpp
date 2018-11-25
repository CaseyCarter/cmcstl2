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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/fundamental.hpp>

STL2_OPEN_NAMESPACE {
	template<class G>
	STL2_CONCEPT UniformRandomNumberGenerator =
		requires(G&& g) {
			g();
			requires UnsignedIntegral<decltype(g())>;
			{ G::min() } -> Same<decltype(g())>&&;
			{ G::max() } -> Same<decltype(g())>&&;
		};
} STL2_CLOSE_NAMESPACE

#endif
