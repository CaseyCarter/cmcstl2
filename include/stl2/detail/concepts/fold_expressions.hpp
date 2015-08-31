#ifndef STL2_DETAIL_CONCEPTS_FOLD_EXPRESSIONS_HPP
#define STL2_DETAIL_CONCEPTS_FOLD_EXPRESSIONS_HPP

#include <stl2/detail/concepts/object.hpp>

// Hack around the absence of fold expressions
namespace stl2 { inline namespace v1 { namespace detail {
// Destructible<Ts>() && ...
template <class...Ts>
constexpr bool AllDestructible = false;
template <Destructible...Ts>
constexpr bool AllDestructible<Ts...> = true;

// MoveConstructible<Ts>() && ...
template <class...Ts>
constexpr bool AllMoveConstructible = false;
template <MoveConstructible...Ts>
constexpr bool AllMoveConstructible<Ts...> = true;

// Movable<Ts>() && ...
template <class...Ts>
constexpr bool AllMovable = false;
template <Movable...Ts>
constexpr bool AllMovable<Ts...> = true;

// CopyConstructible<Ts>() && ...
template <class...Ts>
constexpr bool AllCopyConstructible = false;
template <CopyConstructible...Ts>
constexpr bool AllCopyConstructible<Ts...> = true;

// Copyable<Ts>() && ...
template <class...Ts>
constexpr bool AllCopyable = false;
template <Copyable...Ts>
constexpr bool AllCopyable<Ts...> = true;

// Swappable<Ts>() && ...
template <class...Ts>
constexpr bool AllSwappable = false;
template <Swappable...Ts>
constexpr bool AllSwappable<Ts...> = true;

// EqualityComparable<Ts>() && ...
template <class...Ts>
constexpr bool AllEqualityComparable = false;
template <EqualityComparable...Ts>
constexpr bool AllEqualityComparable<Ts...> = true;

// TotallyOrdered<Ts>() && ...
template <class...Ts>
constexpr bool AllTotallyOrdered = false;
template <TotallyOrdered...Ts>
constexpr bool AllTotallyOrdered<Ts...> = true;
}}} // namespace stl2::v1::detail

#endif
