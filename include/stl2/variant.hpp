#ifndef STL2_VARIANT_HPP
#define STL2_VARIANT_HPP

#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <meta/meta.hpp>
#include <stl2/functional.hpp>
#include <stl2/tuple.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/construct_destruct.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/tuple_like.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/fold_expressions.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/variant/fwd.hpp>
#include <stl2/detail/variant/storage.hpp>
#include <stl2/detail/variant/visit.hpp>

///////////////////////////////////////////////////////////////////////////
// N4542 Variant implementation
//
namespace stl2 { inline namespace v1 { namespace __variant {
struct copy_move_tag {};

template <class T>
constexpr auto& strip_cv(T& t) noexcept {
  return const_cast<remove_cv_t<T>&>(t);
}

///////////////////////////////////////////////////////////////////////////
// Variant access
//
template <std::size_t I, Variant V, _IsNot<is_void> T>
constexpr auto&& v_access::raw_get(meta::size_t<I> i, V&& v) noexcept {
  STL2_VISIT_ASSERT(I == v.index());
  return st_access::raw_get(i, stl2::forward<V>(v).storage_);
}

template <std::size_t I, Variant V, _IsNot<is_void> T>
constexpr auto&& v_access::cooked_get(meta::size_t<I> i, V&& v) noexcept {
  //assert(I == v.index());
  return cook<T>(v_access::raw_get(i, stl2::forward<V>(v)));
}

// "inline" is here for the ODR; we do not actually
// want bad_access to be inlined into get. Having it
// be a separate function results in better generated
// code.
[[noreturn]] inline bool bad_access() {
  throw bad_variant_access{};
}

template <std::size_t I, Variant V>
  requires I < VariantTypes<V>::size() &&
    _IsNot<meta::at_c<VariantTypes<V>, I>, is_void>
constexpr auto&& get_unchecked(V&& v) {
  //assert(v.index() == I);
  return v_access::cooked_get(meta::size_t<I>{}, stl2::forward<V>(v));
}

template <std::size_t I, Variant V>
  requires I < VariantTypes<V>::size() &&
    _IsNot<meta::at_c<VariantTypes<V>, I>, is_void>
constexpr auto&& get(V&& v) {
  assert(v.valid());
  // Odd syntax here to avoid
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67371
  v.index() == I || bad_access();
  return v_access::cooked_get(meta::size_t<I>{}, stl2::forward<V>(v));
}

template <_IsNot<is_void> T, Variant V,
  std::size_t I = index_of_type<T, VariantTypes<V>>>
constexpr auto&& get_unchecked(V&& v) {
  return __variant::get_unchecked<I>(v);
}

template <_IsNot<is_void> T, Variant V,
  std::size_t I = index_of_type<T, VariantTypes<V>>>
constexpr auto&& get(V&& v) {
  return __variant::get<I>(v);
}

template <std::size_t I, Variant V>
  requires I < VariantTypes<V>::size() &&
    _IsNot<meta::at_c<VariantTypes<V>, I>, is_void>
constexpr auto get(V* v) noexcept ->
  decltype(&__variant::v_access::cooked_get(meta::size_t<I>{}, *v)) {
  assert(v);
  if (v->index() == I) {
    return &__variant::v_access::cooked_get(meta::size_t<I>{}, *v);
  }
  return nullptr;
}

template <_IsNot<is_void> T, Variant V,
  std::size_t I = index_of_type<T, VariantTypes<V>>>
constexpr auto get(V* v) noexcept {
  return __variant::get<I>(v);
}

///////////////////////////////////////////////////////////////////////////
// constructible_from<T, Types...> derives from true_type if a variant with
// element types ...Types is constructible from type T. Member "index" is
// the index of the constructible target type in Types, member "ambiguous"
// is true iff multiple members of Types are constructible from T.
//
template <class From, class To>
concept bool ViableAlternative =
  Same<decay_t<From>, decay_t<To>>() &&
  Constructible<To, From>();

template <class T, std::size_t I, class...Types>
struct constructible_from_ : false_type {};

template <class T, std::size_t I, class First, class...Rest>
struct constructible_from_<T, I, First, Rest...> :
  constructible_from_<T, I + 1, Rest...> {};

template <class T, std::size_t I, class First, class...Rest>
  requires ViableAlternative<T, First>
struct constructible_from_<T, I, First, Rest...> : true_type {
  static constexpr bool ambiguous =
     meta::_v<constructible_from_<T, I + 1, Rest...>>;
  static constexpr std::size_t index = I;
};

template <class T, class...Types>
using constructible_from =
  constructible_from_<T, 0u, Types...>;

///////////////////////////////////////////////////////////////////////////
// __variant::base: lowest layer of the variant implementation.
//
template <class...Ts>
  requires detail::AllDestructible<element_t<Ts>...>
class base {
  friend v_access;
protected:
  using storage_t = storage<element_t<Ts>...>;
  using index_t =
#if 1
    std::size_t;
#else
    meta::if_c<
      (sizeof...(Ts) <= std::numeric_limits<std::int_fast8_t>::max()),
      std::int_fast8_t,
      meta::if_c<
        (sizeof...(Ts) <= std::numeric_limits<std::int_fast16_t>::max()),
        std::int_fast16_t,
        meta::if_c<
          (sizeof...(Ts) <= std::numeric_limits<std::int_fast32_t>::max()),
          std::int_fast32_t,
          meta::if_c<
            (sizeof...(Ts) <= std::numeric_limits<std::int_fast64_t>::max()),
            std::int_fast64_t,
            std::size_t>>>>;
#endif
  static_assert(sizeof...(Ts) - is_unsigned<index_t>::value <
                std::numeric_limits<index_t>::max());
  static constexpr auto invalid_index = index_t(-1);

  storage_t storage_;
  index_t index_;

protected:
  void clear() noexcept {
    if (valid()) {
      raw_visit(detail::destruct, *this);
      index_ = invalid_index;
    }
  }

  void clear() noexcept
    requires ext::TriviallyDestructible<storage_t>() {
    index_ = invalid_index;
  }

  template <class That>
    requires DerivedFrom<__uncvref<That>, base>()
  void copy_move_from(That&& that) {
    assert(!valid());
    if (that.valid()) {
      raw_visit_with_index([this](auto i, auto&& from) {
        detail::construct(
          strip_cv(st_access::raw_get(i, storage_)),
            stl2::forward<decltype(from)>(from));
        index_ = i;
      }, stl2::forward<That>(that));
    }
  }

  void move_assign_from(DerivedFrom<base>&& that) {
    if (index_ == that.index_) {
      if (valid()) {
        raw_visit_with_index([this](auto i, auto&& from) {
          st_access::raw_get(i, storage_) =
            stl2::forward<decltype(from)>(from);
        }, stl2::move(that));
      }
    } else {
      clear();
      copy_move_from(stl2::move(that));
    }
  }

  void copy_assign_from(const DerivedFrom<base>& that) {
    if (index_ == that.index_) {
      if (valid()) {
        raw_visit_with_index([this](auto i, const auto& from) {
          st_access::raw_get(i, storage_) = from;
        }, that);
      }
    } else {
      auto tmp = that;
      clear();
      copy_move_from(stl2::move(tmp));
    }
  }

  template <class That>
    requires DerivedFrom<__uncvref<That>, base>()
  base(copy_move_tag, That&& that) :
    storage_{empty_tag{}}, index_{invalid_index} {
    copy_move_from(stl2::forward<That>(that));
  }

public:
  using types = meta::list<Ts...>;

  constexpr base()
    noexcept(is_nothrow_default_constructible<storage_t>::value)
    requires DefaultConstructible<storage_t>() :
    index_{0} {}

  template <class T>
    requires !Same<decay_t<T>, base>() &&
      constructible_from<T&&, Ts...>::value &&
      !constructible_from<T&&, Ts...>::ambiguous &&
      Constructible<storage_t, emplaced_index_t<constructible_from<T&&, Ts...>::index>, T&&>()
  constexpr base(T&& t)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<constructible_from<T&&, Ts...>::index>, T&&>::value)
    : storage_{emplaced_index<constructible_from<T&&, Ts...>::index>, stl2::forward<T>(t)},
      index_{constructible_from<T&&, Ts...>::index} {}

  template <class T>
    requires !Same<decay_t<T>, base>() &&
      constructible_from<T&&, Ts...>::value &&
      !constructible_from<T&&, Ts...>::ambiguous
  constexpr base(T&& t)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<constructible_from<T&&, Ts...>::index>, T&>::value)
    : storage_{emplaced_index<constructible_from<T&&, Ts...>::index>, t},
      index_{constructible_from<T&&, Ts...>::index} {}

  template <class T>
    requires !Same<decay_t<T>, base>() &&
      constructible_from<T&&, Ts...>::value &&
      constructible_from<T&&, Ts...>::ambiguous
  base(T&&) = delete; // Conversion from T is ambiguous.

  template <std::size_t I, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_index_t<I>, Args&&...args)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<I>, Args...>::value)
    : storage_{emplaced_index<I>, stl2::forward<Args>(args)...}, index_{I} {}

  template <std::size_t I, class...Args, _Is<is_reference> T = meta::at_c<types, I>>
  explicit constexpr base(emplaced_index_t<I>, meta::id_t<T> t)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<I>, T&>::value)
    : storage_{emplaced_index<I>, t}, index_{I} {}

  template <_IsNot<is_reference> T, class...Args, std::size_t I = index_of_type<T, types>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_type_t<T>, Args&&...args)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<I>, Args...>::value)
    : storage_{emplaced_index<I>, stl2::forward<Args>(args)...}, index_{I} {}

  template <_Is<is_reference> T, std::size_t I = index_of_type<T, types>>
  explicit constexpr base(emplaced_type_t<T>, meta::id_t<T> t)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<I>, T&>::value)
    : storage_{emplaced_index<I>, t}, index_{I} {}

  template <std::size_t I, class E, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_index_t<I>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<I>, std::initializer_list<E>, Args...>::value)
    : storage_{emplaced_index<I>, il, stl2::forward<Args>(args)...}, index_{I} {}

  template <_IsNot<is_reference> T, class E, class...Args, std::size_t I = index_of_type<T, types>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_type_t<T>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<storage_t, emplaced_index_t<I>, std::initializer_list<E>, Args...>::value)
    : storage_{emplaced_index<I>, il, stl2::forward<Args>(args)...}, index_{I} {}

  template <_IsNot<is_reference> T, class...Args, std::size_t I = index_of_type<T, types>>
    requires Constructible<T, Args...>()
  void emplace(Args&&...args)
    noexcept(is_nothrow_constructible<element_t<T>, Args...>::value) {
    clear();
    detail::construct(
      strip_cv(st_access::raw_get(meta::size_t<I>{}, storage_)),
        stl2::forward<Args>(args)...);
    index_ = I;
  }

  template <_Is<is_reference> T, std::size_t I = index_of_type<T, types>>
  void emplace(meta::id_t<T> t)
    noexcept(is_nothrow_constructible<element_t<T>, T&>::value) {
    clear();
    detail::construct(st_access::raw_get(meta::size_t<I>{}, storage_), t);
    index_ = I;
  }

  template <_IsNot<is_reference> T, class E, class...Args, std::size_t I = index_of_type<T, types>>
    requires Constructible<T, std::initializer_list<E>, Args...>()
  void emplace(std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<element_t<T>, std::initializer_list<E>, Args...>::value) {
    clear();
    detail::construct(
      strip_cv(st_access::raw_get(meta::size_t<I>{}, storage_)),
        il, stl2::forward<Args>(args)...);
    index_ = I;
  }

  template <std::size_t I, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, Args...>()
  void emplace(Args&&...args)
    noexcept(is_nothrow_constructible<element_t<T>, Args...>::value) {
    clear();
    detail::construct(
      strip_cv(st_access::raw_get(meta::size_t<I>{}, storage_)),
        stl2::forward<Args>(args)...);
    index_ = I;
  }

  template <std::size_t I, class...Args, _Is<is_reference> T = meta::at_c<types, I>>
  void emplace(meta::id_t<T> t)
    noexcept(is_nothrow_constructible<element_t<T>, T&>::value) {
    clear();
    detail::construct(st_access::raw_get(meta::size_t<I>{}, storage_), t);
    index_ = I;
  }

  template <std::size_t I, class E, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, std::initializer_list<E>, Args...>()
  void emplace(std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<element_t<T>, std::initializer_list<E>, Args...>::value) {
    clear();
    detail::construct(
      strip_cv(st_access::raw_get(meta::size_t<I>{}, storage_)),
        il, stl2::forward<Args>(args)...);
    index_ = I;
  }

  constexpr std::size_t index() const noexcept {
    return index_;
  }
  constexpr bool valid() const noexcept
    requires is_unsigned<index_t>::value {
     // Consider index_ < invalid_index
    return index_ != invalid_index;
  }
  constexpr bool valid() const noexcept
    requires is_signed<index_t>::value {
    return index_ >= 0;
  }

  template <std::size_t I, Variant V>
    requires I < VariantTypes<V>::size() &&
      _IsNot<meta::at_c<VariantTypes<V>, I>, is_void>
  friend constexpr auto&& get(V&& v);
};

///////////////////////////////////////////////////////////////////////////
// destruct_base: adds nontrival destruction onto base if necessary.
//
template <class...Ts>
class destruct_base : public base<Ts...> {
  using base_t = base<Ts...>;
public:
  ~destruct_base() noexcept {
    this->clear();
  }

  destruct_base() = default;
  destruct_base(destruct_base&&) = default;
  destruct_base(const destruct_base&) = default;
  destruct_base& operator=(destruct_base&&) & = default;
  destruct_base& operator=(const destruct_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires ext::TriviallyDestructible<storage<element_t<Ts>...>>()
class destruct_base<Ts...> : public base<Ts...> {
  using base_t = base<Ts...>;
public:
  using base_t::base_t;
};

///////////////////////////////////////////////////////////////////////////
// move_base: Adds nontrivial or disables move construction.
//
template <class...Ts>
class move_base : public destruct_base<Ts...> {
  using base_t = destruct_base<Ts...>;
public:
  move_base() = default;
  move_base(move_base&&) = delete;
  move_base(const move_base&) = default;
  move_base& operator=(move_base&&) & = default;
  move_base& operator=(const move_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllMoveConstructible<element_t<Ts>...>
class move_base<Ts...> : public destruct_base<Ts...> {
  using base_t = destruct_base<Ts...>;
public:
  move_base() = default;
  move_base(move_base&& that)
    noexcept(meta::_v<meta::all_of<meta::list<element_t<Ts>...>,
               meta::quote_trait<is_nothrow_move_constructible>>>) :
    base_t{copy_move_tag{}, stl2::move(that)} {}
  move_base(const move_base&) = default;
  move_base& operator=(move_base&&) & = default;
  move_base& operator=(const move_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllMoveConstructible<element_t<Ts>...> &&
    ext::TriviallyMoveConstructible<storage<element_t<Ts>...>>()
class move_base<Ts...> : public destruct_base<Ts...> {
  using base_t = destruct_base<Ts...>;
public:
  using base_t::base_t;
};

///////////////////////////////////////////////////////////////////////////
// move_assign_base: adds nontrivial or disables move assignment.
//
template <class...Ts>
class move_assign_base : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  move_assign_base() = default;
  move_assign_base(move_assign_base&&) = default;
  move_assign_base(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) & = delete;
  move_assign_base& operator=(const move_assign_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllMovable<element_t<Ts>...>
class move_assign_base<Ts...> : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  move_assign_base() = default;
  move_assign_base(move_assign_base&&) = default;
  move_assign_base(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&& that) &
    noexcept(meta::_v<meta::fast_and<
      meta::all_of<meta::list<element_t<Ts>...>,
        meta::quote_trait<is_nothrow_move_assignable>>,
      meta::all_of<meta::list<element_t<Ts>...>,
        meta::quote_trait<is_nothrow_move_constructible>>>>) {
    this->move_assign_from(stl2::move(that));
    return *this;
  }
  move_assign_base& operator=(const move_assign_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllMovable<element_t<Ts>...> &&
    ext::TriviallyMovable<storage<element_t<Ts>...>>()
class move_assign_base<Ts...> : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  using base_t::base_t;
};

///////////////////////////////////////////////////////////////////////////
// copy_base: Adds nontrivial or disables copy construction.
//
template <class...Ts>
class copy_base : public move_assign_base<Ts...> {
  using base_t = move_assign_base<Ts...>;
public:
  copy_base() = default;
  copy_base(copy_base&&) = default;
  copy_base(const copy_base&) = delete;
  copy_base& operator=(copy_base&&) & = default;
  copy_base& operator=(const copy_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllCopyConstructible<element_t<Ts>...>
class copy_base<Ts...> : public move_assign_base<Ts...> {
  using base_t = move_assign_base<Ts...>;
public:
  copy_base() = default;
  copy_base(copy_base&&) = default;
  copy_base(const copy_base& that)
    noexcept(meta::_v<meta::all_of<meta::list<element_t<Ts>...>,
               meta::quote_trait<is_nothrow_copy_constructible>>>) :
    base_t{copy_move_tag{}, that} {}
  copy_base& operator=(copy_base&&) & = default;
  copy_base& operator=(const copy_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllCopyConstructible<element_t<Ts>...> &&
    ext::TriviallyCopyConstructible<storage<element_t<Ts>...>>()
class copy_base<Ts...> : public move_assign_base<Ts...> {
  using base_t = move_assign_base<Ts...>;
public:
  using base_t::base_t;
};

///////////////////////////////////////////////////////////////////////////
// copy_assign_base: Adds nontrivial or disables copy assignment.
//
template <class...Ts>
class copy_assign_base : public copy_base<Ts...> {
  using base_t = copy_base<Ts...>;
public:
  copy_assign_base() = default;
  copy_assign_base(copy_assign_base&&) = default;
  copy_assign_base(const copy_assign_base&) = default;
  copy_assign_base& operator=(copy_assign_base&&) & = default;
  copy_assign_base& operator=(const copy_assign_base&) & = delete;

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllCopyable<element_t<Ts>...>
class copy_assign_base<Ts...> : public copy_base<Ts...> {
  using base_t = copy_base<Ts...>;
public:
  copy_assign_base() = default;
  copy_assign_base(copy_assign_base&&) = default;
  copy_assign_base(const copy_assign_base&) = default;
  copy_assign_base& operator=(copy_assign_base&&) & = default;
  copy_assign_base& operator=(const copy_assign_base& that) &
    noexcept(meta::_v<meta::fast_and<
      meta::all_of<meta::list<element_t<Ts>...>,
        meta::quote_trait<is_nothrow_copy_assignable>>,
      meta::all_of<meta::list<element_t<Ts>...>,
        meta::quote_trait<is_nothrow_copy_constructible>>>>) {
    this->copy_assign_from(that);
    return *this;
  }

  using base_t::base_t;
};

template <class...Ts>
  requires detail::AllCopyable<element_t<Ts>...> &&
    ext::TriviallyCopyable<storage<element_t<Ts>...>>()
class copy_assign_base<Ts...> : public copy_base<Ts...> {
  using base_t = copy_base<Ts...>;
public:
  using base_t::base_t;
};
} // namespace __variant

///////////////////////////////////////////////////////////////////////////
// variant: top layer of the variant implementation. Adds swap, comparison
// operators, and converting assignments.
//
template <class...Ts>
  requires detail::AllDestructible<__variant::element_t<Ts>...>
class variant : public __variant::copy_assign_base<Ts...> {
  using base_t = __variant::copy_assign_base<Ts...>;

  template <class T>
  using constructible_from = __variant::constructible_from<T, Ts...>;

  struct swap_visitor {
    variant& self_;

    constexpr void operator()(auto i, auto& from)
      noexcept(is_nothrow_swappable_v<decltype((from)), decltype((from))>) {
      stl2::swap(__variant::v_access::raw_get(i, self_), from);
    }
  };

  struct equal_to_visitor {
    const variant& self_;

    constexpr bool operator()(auto i, const auto& o) const
      noexcept(noexcept(o == o)) {
      const auto& s = __variant::v_access::cooked_get(i, self_);
      static_assert(is_same<decltype(o), decltype(s)>());
      return s == o;
    }
  };

  struct less_than_visitor {
    const variant& self_;

    constexpr bool operator()(auto i, const auto& o) const
      noexcept(noexcept(o < o)) {
      const auto& s = __variant::v_access::cooked_get(i, self_);
      static_assert(is_same<decltype(o), decltype(s)>());
      return s < o;
    }
  };

public:
  using types = meta::list<Ts...>;
  using base_t::base_t;

  template <_IsNot<is_reference> T, class CF = constructible_from<T&&>>
    requires CF::value && !CF::ambiguous &&
      Same<T, meta::at_c<types, CF::index>>() &&
      Movable<T>()
  variant& operator=(T&& t) &
    noexcept(is_nothrow_move_constructible<T>::value &&
             is_nothrow_move_assignable<T>::value) {
    constexpr auto I = CF::index;
    if (this->index_ == I) {
      auto& target = __variant::v_access::raw_get(meta::size_t<I>{}, *this);
      target = stl2::move(t);
    } else {
      this->template emplace<T>(stl2::move(t));
    }
    return *this;
  }

  template <_IsNot<is_reference> T, class CF = constructible_from<const T&>>
    requires CF::value && !CF::ambiguous &&
      Same<T, meta::at_c<types, CF::index>>() &&
      Copyable<T>()
  variant& operator=(const T& t) &
    noexcept(is_nothrow_copy_constructible<T>::value &&
             is_nothrow_copy_assignable<T>::value &&
             is_nothrow_move_constructible<T>::value) {
    constexpr auto I = CF::index;
    if (this->index_ == I) {
      auto& target = __variant::v_access::raw_get(meta::size_t<I>{}, *this);
      target = t;
    } else {
      this->template emplace<T>(T{t});
    }
    return *this;
  }

  template <class T, class CF = constructible_from<T&&>>
    requires CF::value && CF::ambiguous
  variant& operator=(T&&) & = delete; // Assignment from T is ambiguous.

  template <class T, class CF = constructible_from<T&&>>
    requires CF::value && !CF::ambiguous &&
      _Is<meta::at_c<types, CF::index>, is_reference>
  variant& operator=(T&&) & = delete; // Assignment to reference alternatives is ill-formed.

  constexpr void swap(variant& that)
    noexcept(is_nothrow_move_constructible<variant>::value &&
             is_nothrow_move_assignable<variant>::value &&
             meta::_v<meta::and_c<is_nothrow_swappable_v<
               __variant::element_t<Ts>&, __variant::element_t<Ts>&>...>>)
    requires Movable<base_t>() && // Movable<variant>() explodes here.
      detail::AllSwappable<__variant::element_t<Ts>&...>
  {
    if (this->index_ == that.index_) {
      if (this->valid()) {
        __variant::raw_visit_with_index(swap_visitor{*this}, that);
      }
    } else {
      stl2::swap(static_cast<base_t&>(*this),
                 static_cast<base_t&>(that));
    }
  }

  friend constexpr void swap(variant& lhs, variant& rhs)
    noexcept(noexcept(lhs.swap(rhs)))
    requires requires { lhs.swap(rhs); }
  {
    lhs.swap(rhs);
  }

  friend constexpr bool operator==(const variant& lhs, const variant& rhs)
    requires detail::AllEqualityComparable<__variant::element_t<Ts>...>
  {
    if (lhs.index_ != rhs.index_) {
      return false;
    }
    return __variant::visit_with_index(equal_to_visitor{lhs}, rhs);
  }

  friend constexpr bool operator!=(const variant& lhs, const variant& rhs)
    requires detail::AllEqualityComparable<__variant::element_t<Ts>...>
  {
    return !(lhs == rhs);
  }

  friend constexpr bool operator<(const variant& lhs, const variant& rhs)
    requires detail::AllTotallyOrdered<__variant::element_t<Ts>...>
  {
    if (lhs.index_ < rhs.index_) {
      return true;
    } else if (lhs.index_ == rhs.index_) {
      return __variant::visit_with_index(less_than_visitor{lhs}, rhs);
    } else {
      return false;
    }
  }

  friend constexpr bool operator>(const variant& lhs, const variant& rhs)
    requires detail::AllTotallyOrdered<__variant::element_t<Ts>...>
  {
    return rhs < lhs;
  }

  friend constexpr bool operator<=(const variant& lhs, const variant& rhs)
    requires detail::AllTotallyOrdered<__variant::element_t<Ts>...>
  {
    return !(rhs < lhs);
  }

  friend constexpr bool operator>=(const variant& lhs, const variant& rhs)
    requires detail::AllTotallyOrdered<__variant::element_t<Ts>...>
  {
    return !(lhs < rhs);
  }
};

template <>
class variant<> {
  variant() requires false;
};

using __variant::get;
using __variant::get_unchecked;

template <TaggedType...Ts>
using tagged_variant =
  tagged<variant<__tag_elem<Ts>...>, __tag_spec<Ts>...>;

template <class T, __variant::Variant V>
constexpr std::size_t tuple_find<T, V> =
  __variant::index_of_type<T, __variant::VariantTypes<V>>;
}} // namespace stl2::v1

namespace std {
template <::stl2::__variant::Variant V>
struct tuple_size<V> :
  ::meta::size<::stl2::__variant::VariantTypes<V>> {};

template <size_t I, ::stl2::__variant::Variant V>
struct tuple_element<I, V> :
  ::meta::at_c<::stl2::__variant::VariantTypes<V>, I> {};
}

#endif
