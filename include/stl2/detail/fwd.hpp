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
// Workaround GCC PR66957 by declaring this unnamed namespace inline.
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

// Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67384
// Use the expression constraint "deduce_auto_ref_ref(E);" in place
// of the compound constraint "{ E } -> auto&&;"
namespace stl2 { inline namespace v1 { namespace detail {
void deduce_auto_ref(auto&);
void deduce_auto_ref_ref(auto&&);
}}}

#define STL2_DEDUCE_AUTO_REF(E) \
  ::stl2::detail::deduce_auto_ref(E)

#define STL2_DEDUCE_AUTO_REF_REF(E) \
  ::stl2::detail::deduce_auto_ref_ref(E)

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

#define STL2_NOEXCEPT_RETURN(...) \
  noexcept(noexcept(__VA_ARGS__)) \
  { return __VA_ARGS__; }

#if defined(NDEBUG) && defined(__GNUC__)
// Tell the compiler to optimize on the assumption that the condition holds.
#define STL2_ASSERT(...) (void(!(__VA_ARGS__) && (__builtin_unreachable(), true)))
#elif defined(NDEBUG)
#define STL2_ASSERT(...) (void())
#else
#include <cassert>
#define STL2_ASSERT(...) assert(__VA_ARGS__)
#endif

// Hack asserts that ICE the compiler when
// interpreting constexpr functions.
// (Probably https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66635)
#ifdef NDEBUG
#define STL2_BROKEN_ASSERT(...) STL2_ASSERT(__VA_ARGS__)
#else
#define STL2_BROKEN_ASSERT(...) (void(!(__VA_ARGS__) && (std::terminate(), true)))
#endif

#endif
