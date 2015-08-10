#ifndef STL2_DETAIL_FWD_HPP
#define STL2_DETAIL_FWD_HPP

#include <utility>

#include <meta/meta.hpp>

namespace stl2 { inline namespace v1 {

using std::declval;
using std::forward;
using std::move;

namespace detail {
using meta::detail::uncvref_t;
}

namespace ext { namespace models {}}

}} // namespace stl2::v1

// Workaround GCC's bugs with deduction constraints.
#if 0
#define STL2_DEDUCTION_CONSTRAINT(E, ...) { E } -> __VA_ARGS__
#define STL2_EXACT_TYPE_CONSTRAINT(E, ...) STL2_DEDUCTION_CONSTRAINT(E, Same<__VA_ARGS__>)
#define STL2_CONVERSION_CONSTRAINT(E, ...) STL2_DEDUCTION_CONSTRAINT(E, ConvertibleTo<__VA_ARGS__>)
#else
#define STL2_DEDUCTION_CONSTRAINT(E, ...) requires __VA_ARGS__ <decltype(E)>()
#define STL2_EXACT_TYPE_CONSTRAINT(E, ...) requires Same<decltype(E), __VA_ARGS__>()
#define STL2_CONVERSION_CONSTRAINT(E, ...) requires ConvertibleTo<decltype(E),__VA_ARGS__>()
#endif

// For constraining the result of return type deduction,
// which GCC does not currently implement completely.
#if 0
#define STL2_CONSTRAINED_RETURN(...) __VA_ARGS__
#else
#define STL2_CONSTRAINED_RETURN(...) auto
#endif

#endif
