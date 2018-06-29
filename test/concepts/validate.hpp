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
#ifndef CONCEPTS_VALIDATE_HPP
#define CONCEPTS_VALIDATE_HPP

#if VALIDATE_RANGES == VALIDATE_STL2
#error You must define exactly one of VALIDATE_RANGES or VALIDATE_STL2.
#endif

#if VALIDATE_RANGES
#include <range/v3/utility/concepts.hpp>

#define STL2_ASSERT(...) RANGES_ASSERT(__VA_ARGS__)
#define STL2_EXPECT(...) RANGES_ASSERT(__VA_ARGS__)

#elif VALIDATE_STL2
#include <stl2/detail/fwd.hpp>

namespace ranges = __stl2;

#if __cpp_static_assert >= 201411
#define CONCEPT_ASSERT(...) static_assert(__VA_ARGS__)
#else
#define CONCEPT_ASSERT(...) static_assert((__VA_ARGS__), "Concept check failed: " # __VA_ARGS__)
#endif
#endif

#endif // CONCEPTS_VALIDATE_HPP
