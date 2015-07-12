#ifndef STL2_CONCEPTS_FOUNDATIONAL_HPP
#define STL2_CONCEPTS_FOUNDATIONAL_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/core.hpp>

#include <type_traits>
#include <utility>

////////////////////////
// Foundational Concepts
//
namespace stl2 { inline namespace v1 {

template <class T>
concept bool Destructible() { return
    std::is_object<T>::value &&
    requires(T& t, T* p) {
      { t.~T() } noexcept;
      &t;
      requires Same<T*, decltype(&t)>;
      delete p;
      delete[] p;
    };
}

template <class T, class...Args>
concept bool Constructible() { return
  Destructible<T>() &&
  requires(Args&&...args) {
    T{ stl2::forward<Args>(args)... };
    new T{ stl2::forward<Args>(args)... };
  };
}

template <class T>
concept bool MoveConstructible =
  Constructible<T, T&&>();

template <class T>
concept bool CopyConstructible =
  MoveConstructible<T> &&
  Constructible<T, T&>() &&
  Constructible<T, const T&>() &&
  Constructible<T, const T&&>();

template <class T>
concept bool Movable =
  MoveConstructible<T> &&
  Assignable<T&, T&&>;

template <class T>
concept bool Copyable =
  Movable<T> &&
  CopyConstructible<T> &&
  Assignable<T&, T&> &&
  Assignable<T&, const T&> &&
  Assignable<T&, const T&&>;

template <class T>
concept bool Semiregular =
  Copyable<T> &&
  Constructible<T>() &&
  requires(std::size_t n) {
    new T[n];
    requires Same<T*, decltype(new T[n])>;
  };

template <Movable T, AssignableTo<T&> U = T>
constexpr T exchange(T& t, U&& u)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_assignable<T&, U>::value);

Movable{T}
constexpr void swap(T& a, T& b)
  noexcept(noexcept(b = exchange(a, stl2::move(b))));

namespace detail {
  constexpr struct __try_swap_fn {
    template <class T, class U>
    auto operator()(T &t, U &u) const
      noexcept(noexcept(swap(t,u)))
      -> decltype(swap(t,u));

    template <class T, class U, std::size_t N,
      typename This = __try_swap_fn>
    auto operator()(T (&t)[N], U (&u)[N]) const
      noexcept(noexcept(This{}(*t, *u)))
      -> decltype(This{}(*t, *u));
  } __try_swap{};
}

template <class T, class U, std::size_t N>
  requires requires (T &t, U &u) { detail::__try_swap(t, u); }
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(noexcept(detail::__try_swap(*t, *u)));

template <class T, class U = T>
concept bool Swappable =
  requires (T&& t, U&& u) {
    swap(stl2::forward<T>(t), stl2::forward<U>(u));
    swap(stl2::forward<U>(u), stl2::forward<T>(t));
  };

template <class B>
concept bool Boolean =
  Convertible<B, bool> &&
  requires(B&& b1, B&& b2) {
    // { !b1 } -> Boolean;
    !b1; requires Convertible<decltype(!b1),bool>;
    //{ b1 && b2 } -> Same<bool>;
    b1 && b2; requires Convertible<decltype(b1 && b2),bool>;
    //{ b1 || b2 } -> Same<bool>;
    b1 || b2; requires Convertible<decltype(b1 || b2),bool>;
  };

namespace detail {

template <class T, class U>
concept bool EqualityComparable_ =
  requires(T&& t, U&& u) {
#if 0 // FIXME: ICE
    { stl2::forward<T>(t) == stl2::forward<U>(u) } -> Boolean;
    { stl2::forward<T>(t) != stl2::forward<U>(u) } -> Boolean;
#else
    { stl2::forward<T>(t) == stl2::forward<U>(u) } -> bool;
    { stl2::forward<T>(t) != stl2::forward<U>(u) } -> bool;
#endif
  };

} // namespace detail

template <class T, class U = T>
concept bool EqualityComparable =
  detail::EqualityComparable_<T, T> &&
  (Same<T, U> ||
    (detail::EqualityComparable_<T, U> &&
     detail::EqualityComparable_<U, T> &&
     detail::EqualityComparable_<U, U>));

template <class T, class U = T>
concept bool StronglyEqualityComparable =
  EqualityComparable<T, U> &&
  (Same<T, U> ||
    (Common<T, U> &&
     EqualityComparable<CommonType<T, U>>));

template <class T>
concept bool Regular =
  Semiregular<T> &&
  EqualityComparable<T>;

namespace detail {

template <class T, class U>
concept bool TotallyOrdered_ =
  EqualityComparable<T, U> &&
  requires(T&& a, U&& b) {
#if 0 // FIXME: ICE
    //{ a < b } -> Boolean;
    //{ a > b } -> Boolean;
    //{ a <= b } -> Boolean;
    //{ a >= b } -> Boolean;
#else
    { a < b } -> bool;
    { a > b } -> bool;
    { a <= b } -> bool;
    { a >= b } -> bool;
#endif
  };

} // namespace detail

template <class T, class U = T>
concept bool TotallyOrdered =
  detail::TotallyOrdered_<T, T> &&
  (Same<T, U> ||
    (detail::TotallyOrdered_<T, U> &&
     detail::TotallyOrdered_<U, T> &&
     detail::TotallyOrdered_<U, U>));

template <class T, class U = T>
concept bool StronglyTotallyOrdered =
  TotallyOrdered<T, U> &&
  (Same<T, U> ||
    (Common<T, U> &&
     TotallyOrdered<CommonType<T, U>>));

#if 0
template <class T>
concept bool Scalar =
  std::is_scalar<T>::value && Regular<T>;

template <class T>
concept bool Arithmetic =
  std::is_arithmetic<T>::value && Scalar<T> && TotallyOrdered<T>;

template <class T>
concept bool Integral =
  std::is_integral<T>::value && Arithmetic<T>;

#else

template <class T>
concept bool Integral =
  std::is_integral<T>::value;
#endif

template <class T>
concept bool SignedIntegral =
  Integral<T> && (T(-1) < T(0));

template <class T>
concept bool UnsignedIntegral =
  Integral<T> && !SignedIntegral<T>;

// Integral<T> subsumes SignedIntegral<T> and UnsignedIntegral<T>
// SignedIntegral<T> and UnsignedIntegral<T> are mutually exclusive

namespace models {

template <class>
constexpr bool destructible() { return false; }

Destructible{T}
constexpr bool destructible() { return true; }


template <class, class...>
constexpr bool constructible() { return false; }

Constructible{T, ...Args}
constexpr bool constructible() { return false; }


template <class>
constexpr bool move_constructible() { return false; }

MoveConstructible{T}
constexpr bool move_constructible() { return true; }


template <class>
constexpr bool copy_constructible() { return false; }

CopyConstructible{T}
constexpr bool copy_constructible() { return true; }


template <class>
constexpr bool movable() { return false; }

Movable{T}
constexpr bool movable() { return true; }


template <class>
constexpr bool copyable() { return false; }

Copyable{T}
constexpr bool copyable() { return true; }


template <class>
constexpr bool swappable() { return false; }

template <class T>
  requires Swappable<T>
constexpr bool swappable() { return true; }

template <class, class>
constexpr bool swappable() { return false; }

Swappable{T, U}
constexpr bool swappable() { return true; }


template <class>
constexpr bool equality_comparable() { return false; }

template <class T>
  requires EqualityComparable<T>
constexpr bool equality_comparable() { return true; }

template <class, class>
constexpr bool equality_comparable() { return false; }

EqualityComparable{T, U}
constexpr bool equality_comparable() { return true; }


template <class>
constexpr bool semiregular() { return false; }

Semiregular{T}
constexpr bool semiregular() { return true; }


template <class>
constexpr bool regular() { return false; }

Regular{T}
constexpr bool regular() { return true; }


template <class>
constexpr bool totally_ordered() { return false; }

template <class T>
  requires TotallyOrdered<T>
constexpr bool totally_ordered() { return true; }

template <class, class>
constexpr bool totally_ordered() { return false; }

TotallyOrdered{T, U}
constexpr bool totally_ordered() { return true; }


template <class>
constexpr bool boolean() { return false; }

Boolean{T}
constexpr bool boolean() { return true; }


template <class>
constexpr bool integral() { return false; }

Integral{T}
constexpr bool integral() { return true; }

}}} // namespace stl2::v1::models

#endif // STL2_CONCEPTS_FOUNDATIONAL_HPP
