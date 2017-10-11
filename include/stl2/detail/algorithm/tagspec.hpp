// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_TAGSPEC_HPP
#define STL2_DETAIL_ALGORITHM_TAGSPEC_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
	// tag specifiers [alg.tagspec]
	namespace tag {
		STL2_DEFINE_GETTER(in)
		STL2_DEFINE_GETTER(in1)
		STL2_DEFINE_GETTER(in2)
		STL2_DEFINE_GETTER(out)
		STL2_DEFINE_GETTER(out1)
		STL2_DEFINE_GETTER(out2)
		STL2_DEFINE_GETTER(fun)
		STL2_DEFINE_GETTER(min)
		STL2_DEFINE_GETTER(max)
		STL2_DEFINE_GETTER(begin)
		STL2_DEFINE_GETTER(end)
		STL2_DEFINE_GETTER(count) // Extension
	}
} STL2_CLOSE_NAMESPACE

#endif
