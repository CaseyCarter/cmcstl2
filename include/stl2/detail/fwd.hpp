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

template <class T>
struct static_const {
  static constexpr T value{};
};

template <class T>
constexpr T static_const<T>::value;
}

namespace ext {
template <unsigned N>
struct priority_tag : priority_tag<N - 1> {};
template <>
struct priority_tag<0> {};
inline namespace {
constexpr auto& max_priority_tag = detail::static_const<priority_tag<1>>::value;
}

namespace models {}
}}} // namespace stl2::v1::ext

// Workaround bugs in deduction constraints by replacing:
// * { E } -> T with requires T<decltype(E)>()
// * { E } -> Same<T> with requires Same<decltype(E), T>()
// * { E } -> ConvertibleTo<T> with requires Convertible<decltype(E), T>()
#if 0
#define STL2_DEDUCTION_CONSTRAINT(E, ...) \
  { E } -> __VA_ARGS__

#define STL2_BINARY_DEDUCTION_CONSTRAINT(E, C, ...) \
  STL2_DEDUCTION_CONSTRAINT(E, C<__VA_ARGS__>)

#else
#define STL2_DEDUCTION_CONSTRAINT(E, ...) \
  requires __VA_ARGS__ <decltype(E)>()

#define STL2_BINARY_DEDUCTION_CONSTRAINT(E, C, ...) \
  requires C<decltype(E), __VA_ARGS__>()
#endif

#define STL2_EXACT_TYPE_CONSTRAINT(E, ...) \
  STL2_BINARY_DEDUCTION_CONSTRAINT(E, Same, __VA_ARGS__)

#define STL2_CONVERSION_CONSTRAINT(E, ...) \
  STL2_BINARY_DEDUCTION_CONSTRAINT(E, ConvertibleTo, __VA_ARGS__)

// Workaround bugs in constrained return types
// (e.g., Iterator begin(Range&&);) by simply disabling
// the feature and using "auto"
#if 0
#define STL2_CONSTRAINED_RETURN(...) __VA_ARGS__
#else
#define STL2_CONSTRAINED_RETURN(...) auto
#endif

// Workaround bugs in constrained variable definitions
// (e.g., Iterator x = begin(r);) by simply disabling
// the feature and using "auto"
#if 0
#define STL2_CONSTRAINED_VAR(...) __VA_ARGS__
#else
#define STL2_CONSTRAINED_VAR(...) auto
#endif

#endif
