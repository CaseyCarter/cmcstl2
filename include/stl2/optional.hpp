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
#ifndef STL2_OPTIONAL_HPP
#define STL2_OPTIONAL_HPP

#include <ostream>
#include <stdexcept>
#include <stl2/type_traits.hpp>
#include <stl2/variant.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// Implementation of N4529 optional
//
// TODO:
// * Exception safety audit
// * optional<T&>: Not forbidden since variant supports references, but I
//   need to ensure the semantics are reasonable.
//
STL2_OPEN_NAMESPACE {
  struct nullopt_t {
    struct __tag {};
    constexpr nullopt_t(__tag) {}
  };

  namespace __optional {
    template <class T>
    struct static_const {
      static constexpr T value{nullopt_t::__tag{}};
    };
    template <class T>
    constexpr T static_const<T>::value;
  }
  namespace {
    constexpr auto& nullopt = __optional::static_const<nullopt_t>::value;
  }

  struct in_place_t{};
  namespace {
    constexpr auto& in_place = detail::static_const<in_place_t>::value;
  }

  class bad_optional_access : public std::logic_error {
  public:
    bad_optional_access() :
      logic_error{"Attempt to access disengaged optional"} {}
  };

  namespace __optional {
    using nil_ = monostate;

    [[noreturn]] inline bool bad_access() {
      throw bad_optional_access{};
    }

    Destructible{T}
    class optional;

    struct access {
      template <class O>
      static constexpr decltype(auto) v(O&& o) noexcept {
        return (__stl2::forward<O>(o).v_);
      }
      template <class T>
      static constexpr decltype(auto) cv(const optional<T>& o) noexcept {
        return access::v(o);
      }
    };

    ext::ExplicitlyConvertibleTo{From, To}
    struct narrowing_converter {
      static_assert(is_reference<From>());
      From from_;

      constexpr operator To() &&
      STL2_NOEXCEPT_RETURN(
        static_cast<To>(std::forward<From>(from_))
      )
    };

    template <class To, class From>
      requires ext::ExplicitlyConvertibleTo<From, To>()
    constexpr auto allow_narrowing_conversion(From&& f) noexcept {
      return narrowing_converter<From&&, To>{__stl2::forward<From>(f)};
    }

    template <class T>
    struct convert_visitor {
      constexpr auto operator()(nil_) const noexcept {
        return variant<nil_, T>{};
      }
      template <ConvertibleTo<T> U>
      constexpr auto operator()(U&& u) const
      STL2_NOEXCEPT_RETURN(
        variant<nil_, T>{emplaced_type<T>,
          allow_narrowing_conversion(__stl2::forward<U>(u))}
      )
    };

    Destructible{T}
    class optional {
      friend struct access;
      variant<nil_, T> v_;

      template <class U>
      static constexpr decltype(auto) get_unchecked(U&& v) noexcept {
        STL2_CONSTEXPR_ASSUME(__stl2::holds_alternative<T>(v));
        return __stl2::get_unchecked<T>(__stl2::forward<decltype(v)>(v));
      }

      template <class U>
      static constexpr decltype(auto) get(U&& v) {
        __stl2::holds_alternative<nil_>(v) && bad_access();
        return __stl2::get_unchecked<T>(__stl2::forward<decltype(v)>(v));
      }

    public:
      using value_type = T;

      static_assert(!is_same<__uncvref<T>, nullopt_t>(),
                    "optional cannot hold nullopt_t");
      static_assert(!is_same<__uncvref<T>, in_place_t>(),
                    "optional cannot hold in_place_t");
      static_assert(!is_same<__uncvref<T>, monostate>(),
                    "optional cannot hold monostate");

      constexpr optional() noexcept = default;
      constexpr optional(nullopt_t) noexcept :
        optional() {}

      constexpr optional(const T& t)
        noexcept(is_nothrow_copy_constructible<T>::value)
        requires CopyConstructible<T>() :
        v_{t} {}
      constexpr optional(T&& t)
        noexcept(is_nothrow_move_constructible<T>::value)
        requires MoveConstructible<T>() :
        v_{__stl2::move(t)} {}

      template <class...Args>
        requires Constructible<T, Args...>()
      constexpr explicit optional(in_place_t, Args&&...args)
        noexcept(is_nothrow_constructible<T, Args...>::value) :
        v_{emplaced_type<T>, __stl2::forward<Args>(args)...} {}

      template <class U, class...Args>
        requires Constructible<T, std::initializer_list<U>&, Args...>()
      constexpr explicit optional(
        in_place_t, std::initializer_list<U> il, Args&&...args)
        noexcept(is_nothrow_constructible<T,
          std::initializer_list<U>&, Args...>::value) :
        v_{emplaced_type<T>, il, __stl2::forward<Args>(args)...} {}

      template <ConvertibleTo<T> U>
        requires Constructible<T, U>()
      constexpr optional(optional<U>&& that)
        noexcept(is_nothrow_constructible<T, U>::value) :
        v_{__stl2::visit(convert_visitor<T>{}, access::v(__stl2::move(that)))} {}

      template <ConvertibleTo<T> U>
      explicit constexpr optional(optional<U>&& that)
        noexcept(is_nothrow_constructible<T, U>::value) :
        v_{__stl2::visit(convert_visitor<T>{}, access::v(__stl2::move(that)))} {}

      template <ConvertibleTo<T> U>
        requires Constructible<T, const U&>()
      constexpr optional(const optional<U>& that)
        noexcept(is_nothrow_constructible<T, const U&>::value) :
        v_{__stl2::visit(convert_visitor<T>{}, access::cv(that))} {}

      template <ConvertibleTo<T> U>
      explicit constexpr optional(const optional<U>& that)
        noexcept(is_nothrow_constructible<T, const U&>::value) :
        v_{__stl2::visit(convert_visitor<T>{}, access::cv(that))} {}

      optional& operator=(nullopt_t) & noexcept {
        v_ = nil_{};
        return *this;
      }

      template <class U>
        requires Same<decay_t<U>, T>() &&
          Constructible<T, U>() &&
          Assignable<T&, U>()
      optional& operator=(U&& u) &
        noexcept(is_nothrow_constructible<T, U>::value &&
                 is_nothrow_assignable<T&, U>::value) {
        if (__stl2::holds_alternative<nil_>(v_)) {
          v_.emplace<T>(__stl2::forward<U>(u));
        } else {
          optional::get_unchecked(v_) = __stl2::forward<U>(u);
        }
        return *this;
      }

      template <ConvertibleTo<T> U>
      optional& operator=(const optional<U>& that) &
        noexcept(is_nothrow_constructible<T, const U&>::value &&
                 is_nothrow_assignable<T&, const U&>::value) {
        if (that) {
          if (*this) {
            **this = *that;
          } else {
            v_.emplace<T>(*that);
          }
        } else {
          *this = {};
        }
        return *this;
      }

      template <ConvertibleTo<T> U>
      optional& operator=(optional<U>&& that) &
        noexcept(is_nothrow_constructible<T, U>::value &&
                 is_nothrow_assignable<T&, U>::value) {
        if (that) {
          if (*this) {
            **this = __stl2::move(*that);
          } else {
            v_.emplace<T>(__stl2::move(*that));
          }
        } else {
          *this = {};
        }
        return *this;
      }

      template <class...Args>
        requires Constructible<T, Args...>()
      void emplace(Args&&...args)
        noexcept(is_nothrow_constructible<T, Args...>::value) {
        v_.emplace<T>(__stl2::forward<Args>(args)...);
      }
      template <class U, class... Args>
        requires Constructible<T, std::initializer_list<U>&, Args...>()
      void emplace(std::initializer_list<U> il, Args&&...args)
        noexcept(is_nothrow_constructible<T,
          std::initializer_list<U>&, Args...>::value) {
        v_.emplace<T>(il, __stl2::forward<Args>(args)...);
      }

      template <class U>
        requires Swappable<T&, U&>() &&
          Constructible<T, U&&>() &&
          Constructible<U, T&&>()
      void swap(optional<U>& that)
        noexcept(is_nothrow_move_constructible<T>::value &&
                 is_nothrow_move_constructible<U>::value &&
                 is_nothrow_swappable_v<T&, U&>) {
        if (*this) {
          if (that) {
            __stl2::swap(**this, *that);
          } else {
            access::v(that).emplace<U>(__stl2::move(**this));
            *this = {};
          }
        } else {
          if (that) {
            v_.emplace<T>(__stl2::move(*that));
            that = {};
          } else {
            // Nothing to do.
          }
        }
      }

      constexpr const T* operator->() const {
        return &optional::get_unchecked(v_);
      }
      constexpr T* operator->() {
        return &optional::get_unchecked(v_);
      }

      constexpr T const& operator*() const & noexcept {
        return optional::get_unchecked(v_);
      }
      constexpr T& operator*() & noexcept {
        return optional::get_unchecked(v_);
      }
      constexpr T&& operator*() && noexcept {
        return optional::get_unchecked(__stl2::move(v_));
      }
      constexpr const T&& operator*() const && {
        return optional::get_unchecked(__stl2::move(v_));
      }

      constexpr explicit operator bool() const noexcept {
        return !__stl2::holds_alternative<nil_>(v_);
      }

      constexpr T const& value() const & {
        return optional::get(v_);
      }
      constexpr T& value() & {
        return optional::get(v_);
      }
      constexpr T&& value() && {
        return optional::get(__stl2::move(v_));
      }
      constexpr const T&& value() const && {
        return optional::get(__stl2::move(v_));
      }

      template <ext::ExplicitlyConvertibleTo<T> U>
        requires CopyConstructible<T>()
      constexpr T value_or(U&& u) const & {
        return *this
          ? **this
          : static_cast<T>(__stl2::forward<U>(u));
      }
      template <ext::ExplicitlyConvertibleTo<T> U>
        requires CopyConstructible<T>()
      constexpr T value_or(U&& u) && {
        return *this
          ? __stl2::move(**this)
          : static_cast<T>(__stl2::forward<U>(u));
      }
    };

    template <class T, class U>
      requires Swappable<T&, U&>()
    void swap(optional<T>& lhs, optional<U>& rhs)
    STL2_NOEXCEPT_RETURN(
      lhs.swap(rhs)
    )

    struct eq_visitor {
      ext::WeaklyEqualityComparable{T, U}
      constexpr bool operator()(const T& l, const U& r) const
      STL2_NOEXCEPT_RETURN(
        l == r
      )
      constexpr bool operator()(const auto&, const auto&) const noexcept {
        return false;
      }
      constexpr bool operator()(nil_, nil_) const noexcept {
        return true;
      }
    };

    ext::WeaklyEqualityComparable{T, U}
    constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs)
    STL2_NOEXCEPT_RETURN(
      __stl2::visit(eq_visitor{}, access::cv(lhs), access::cv(rhs))
    )
    ext::WeaklyEqualityComparable{T, U}
    constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
    STL2_NOEXCEPT_RETURN(
      !(lhs == rhs)
    )

    struct lt_visitor {
      ext::WeaklyTotallyOrdered{T, U}
      constexpr bool operator()(const T& l, const U& r) const
      STL2_NOEXCEPT_RETURN(
        l < r
      )
      constexpr bool operator()(const auto&, const auto&) const noexcept {
        return false;
      }
    };

    ext::WeaklyTotallyOrdered{T, U}
    constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs)
    STL2_NOEXCEPT_RETURN(
      __stl2::visit(lt_visitor{}, access::cv(lhs), access::cv(rhs))
    )

    ext::WeaklyTotallyOrdered{T, U}
    constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs)
    STL2_NOEXCEPT_RETURN(
      rhs < lhs
    )

    ext::WeaklyTotallyOrdered{T, U}
    constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
    STL2_NOEXCEPT_RETURN(
      !(rhs < lhs)
    )

    ext::WeaklyTotallyOrdered{T, U}
    constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
    STL2_NOEXCEPT_RETURN(
      !(lhs < rhs)
    )

    constexpr bool operator==(const optional<auto>& o, nullopt_t) noexcept {
      return !o;
    }
    constexpr bool operator==(nullopt_t, const optional<auto>& o) noexcept {
      return o == nullopt;
    }

    constexpr bool operator!=(const optional<auto>& o, nullopt_t) noexcept {
      return !(o == nullopt);
    }
    constexpr bool operator!=(nullopt_t, const optional<auto>& o) noexcept {
      return o != nullopt;
    }

    constexpr bool operator<(const optional<auto>&, nullopt_t) noexcept {
      return false;
    }
    constexpr bool operator<(nullopt_t, const optional<auto>& o) noexcept {
      return !(o == nullopt);
    }

    constexpr bool operator<=(const optional<auto>& o, nullopt_t) noexcept {
      return o == nullopt;
    }
    constexpr bool operator<=(nullopt_t, const optional<auto>&) noexcept {
      return true;
    }

    constexpr bool operator>(const optional<auto>& o, nullopt_t) noexcept {
      return o != nullopt;
    }
    constexpr bool operator>(nullopt_t, const optional<auto>&) noexcept {
      return false;
    }

    constexpr bool operator>=(const optional<auto>&, nullopt_t) noexcept {
      return true;
    }
    constexpr bool operator>=(nullopt_t, const optional<auto>& o) noexcept {
      return o == nullopt;
    }

    EqualityComparable{T}
    constexpr bool operator==(const optional<T>& o, const T& t)
    STL2_NOEXCEPT_RETURN(
      o ? *o == t : false
    )
    EqualityComparable{T}
    constexpr bool operator==(const T& t, const optional<T>& o)
    STL2_NOEXCEPT_RETURN(
      o == t
    )

    EqualityComparable{T}
    constexpr bool operator!=(const optional<T>& o, const T& t)
    STL2_NOEXCEPT_RETURN(
      !(o == t)
    )
    EqualityComparable{T}
    constexpr bool operator!=(const T& t, const optional<T>& o)
    STL2_NOEXCEPT_RETURN(
      !(o == t)
    )

    TotallyOrdered{T}
    constexpr bool operator<(const optional<T>& o, const T& t)
    STL2_NOEXCEPT_RETURN(
      o ? *o < t : true
    )
    TotallyOrdered{T}
    constexpr bool operator<(const T& t, const optional<T>& o)
    STL2_NOEXCEPT_RETURN(
      o ? t < *o : false
    )

    TotallyOrdered{T}
    constexpr bool operator>(const optional<T>& o, const T& t)
    STL2_NOEXCEPT_RETURN(
      t < o
    )
    TotallyOrdered{T}
    constexpr bool operator>(const T& t, const optional<T>& o)
    STL2_NOEXCEPT_RETURN(
      o < t
    )

    TotallyOrdered{T}
    constexpr bool operator<=(const optional<T>& o, const T& t)
    STL2_NOEXCEPT_RETURN(
      !(t < o)
    )
    TotallyOrdered{T}
    constexpr bool operator<=(const T& t, const optional<T>& o)
    STL2_NOEXCEPT_RETURN(
      !(o < t)
    )

    TotallyOrdered{T}
    constexpr bool operator>=(const optional<T>& o, const T& t)
    STL2_NOEXCEPT_RETURN(
      !(o < t)
    )
    TotallyOrdered{T}
    constexpr bool operator>=(const T& t, const optional<T>& o)
    STL2_NOEXCEPT_RETURN(
      !(t < o)
    )
  } // namespace __optional

  using __optional::optional;

  template <class T>
  constexpr optional<decay_t<T>> make_optional(T&& t)
    noexcept(is_nothrow_constructible<optional<decay_t<T>>, T>::value) {
    return optional<decay_t<T>>(__stl2::forward<T>(t));
  }

  Common{T, U}
  struct common_type<optional<T>, optional<U>> {
    using type = optional<CommonType<T, U>>;
  };
} STL2_CLOSE_NAMESPACE

namespace std {
  ::__stl2::ext::Hashable{T}
  struct hash<::__stl2::optional<T>> {
    using result_type = size_t;
    using argument_type = ::__stl2::optional<T>;

    constexpr size_t operator()(const argument_type& o) const {
      if (o) {
        size_t seed = 0;
        ::__stl2::ext::hash_combine(seed, *o);
        return seed;
      } else {
        return 42;
      }
    }
  };
}

#endif
