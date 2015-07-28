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

#if 0
#define STL2_DEDUCTION_CONSTRAINT(E, ...) { E } -> __VA_ARGS__
#define STL2_EXACT_TYPE_CONSTRAINT(E, ...) STL2_DEDUCTION_CONSTRAINT(E, Same<__VA_ARGS__>)
#define STL2_CONVERSION_CONSTRAINT(E, ...) STL2_DEDUCTION_CONSTRAINT(E, ConvertibleTo<__VA_ARGS__>)
#else
#define STL2_DEDUCTION_CONSTRAINT(E, ...) requires __VA_ARGS__ <decltype(E)>()
#define STL2_EXACT_TYPE_CONSTRAINT(E, ...) requires Same<decltype(E), __VA_ARGS__>()
#define STL2_CONVERSION_CONSTRAINT(E, ...) requires ConvertibleTo<decltype(E),__VA_ARGS__>()
#endif

#define STL2_DECLTYPE_AUTO_RETURN_NOEXCEPT(...) \
  noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) { return (__VA_ARGS__); }

#endif
