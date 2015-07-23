#ifndef CONCEPTS_VALIDATE_HPP
#define CONCEPTS_VALIDATE_HPP

#if VALIDATE_RANGES == VALIDATE_STL2
#error You must define exactly one of VALIDATE_RANGES or VALIDATE_STL2.
#endif

#if VALIDATE_RANGES
#include <range/v3/utility/concepts.hpp>

namespace models {}

#elif VALIDATE_STL2
#include <stl2/detail/fwd.hpp>

namespace models = stl2::ext::models;

#if __cpp_static_assert >= 201411
#define CONCEPT_ASSERT(...) static_assert(__VA_ARGS__)
#else
#define CONCEPT_ASSERT(...) static_assert((__VA_ARGS__), "Concept check failed: " # __VA_ARGS__)
#endif
#endif

#endif // CONCEPTS_VALIDATE_HPP
