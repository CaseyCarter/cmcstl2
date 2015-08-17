#ifndef STL2_VARIANT_HPP
#define STL2_VARIANT_HPP

#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <meta/meta.hpp>
#include <stl2/functional.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>

namespace stl2 { inline namespace v1 {
template <class...>
class variant;

template <class>
struct emplaced_type_t {};

namespace {
template <class T>
constexpr auto& emplaced_type =
  detail::static_const<emplaced_type_t<T>>::value;
}

template <std::size_t I>
using emplaced_index_t = meta::size_t<I>;

namespace {
template <std::size_t I>
constexpr auto& emplaced_index =
  detail::static_const<emplaced_index_t<I>>::value;
}

struct monostate {};

constexpr bool operator==(const monostate&, const monostate&)
{ return true; }
constexpr bool operator!=(const monostate&, const monostate&)
{ return false; }
constexpr bool operator<(const monostate&, const monostate&)
{ return false; }
constexpr bool operator>(const monostate&, const monostate&)
{ return false; }
constexpr bool operator<=(const monostate&, const monostate&)
{ return true; }
constexpr bool operator>=(const monostate&, const monostate&)
{ return true; }

class bad_variant_access {};

namespace __variant {
template <class T, class...Types, class Tail = meta::find<meta::list<Types...>, T>>
  requires !meta::_v<meta::empty<Tail>> &&
  meta::_v<meta::find_index<meta::pop_front<Tail>, T>> == meta::_v<meta::npos>
constexpr std::size_t index_of_type =
  sizeof...(Types) - Tail::size();

template <class...> class base;
}

template <class T, class...Types>
constexpr bool holds_alternative(const __variant::base<Types...>& v) noexcept {
  return v.index() == __variant::index_of_type<T, Types...>;
}

template <std::size_t I, class...Types>
constexpr decltype(auto) get(__variant::base<Types...>& v);

template <std::size_t I, class...Types>
constexpr decltype(auto) get(const __variant::base<Types...>& v);

template <std::size_t I, class...Types>
constexpr decltype(auto) get(__variant::base<Types...>&& v);

namespace __variant {
template <class T>
using element_t =
  meta::if_<is_reference<T>,
    reference_wrapper<remove_reference_t<T>>,
    T>;

struct empty_tag {};

template <bool TrivialDestruction, class...Ts> // Destructible...Ts
union data_;

template <class>
constexpr bool is_data = false;
template <bool B, class...Ts>
constexpr bool is_data<data_<B, Ts...>> = true;

template <class T>
concept bool IsData = is_data<decay_t<T>>;

template <class...Ts>
using data = data_<
  meta::all_of<
    meta::list<Ts...>,
    meta::quote_trait<is_trivially_destructible>>::value,
  Ts...>;

template <bool TrivialDestruction>
union data_<TrivialDestruction> {
  static constexpr std::size_t size = 0;
};


// TODO: Think about binary instead of linear decomposition of the elements, i.e.,
//       a recursive union binary tree instead of a recursive union list. It will
//       add implementation complexity, but may interact positively with inlining.
template <class First, class...Rest>
union data_<true, First, Rest...> {
  static constexpr std::size_t size = 1 + sizeof...(Rest);

  using head_t = First;
  using tail_t = data_<true, Rest...>;
  head_t head_;
  tail_t tail_;

  data_() requires DefaultConstructible<head_t>() = default;

  data_(empty_tag) {}

  template <std::size_t N, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, Args...>()
  constexpr data_(meta::size_t<N>, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, stl2::forward<Args>(args)...} {}

  template <class...Args>
    requires Constructible<First, Args...>()
  constexpr data_(meta::size_t<0>, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<head_t, Args...>::value) :
    head_{stl2::forward<Args>(args)...} {}

  template <std::size_t N, class E, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
  constexpr data_(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, il, stl2::forward<Args>(args)...} {}

  template <class E, class...Args>
    requires Constructible<First, std::initializer_list<E>, Args...>()
  constexpr data_(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
    head_{il, stl2::forward<Args>(args)...} {}
};

template <class First, class...Rest>
union data_<false, First, Rest...> {
  static constexpr std::size_t size = 1 + sizeof...(Rest);

  using head_t = First;
  using tail_t = data_<false, Rest...>;
  head_t head_;
  tail_t tail_;

  ~data_() {}

  constexpr data_() requires DefaultConstructible<head_t>() : head_{} {}

  data_(empty_tag) {}

  template <std::size_t N, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, Args...>()
  constexpr data_(meta::size_t<N>, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, stl2::forward<Args>(args)...} {}

  template <class...Args>
    requires Constructible<First, Args...>()
  constexpr data_(meta::size_t<0>, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<head_t, Args...>::value) :
    head_{stl2::forward<Args>(args)...} {}

  template <std::size_t N, class E, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
  constexpr data_(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, il, stl2::forward<Args>(args)...} {}

  template <class E, class...Args>
    requires Constructible<First, std::initializer_list<E>, Args...>()
  constexpr data_(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
    noexcept(stl2::is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
    head_{il, stl2::forward<Args>(args)...} {}
};

template <IsData D>
constexpr decltype(auto) raw_get(meta::size_t<0>, D&& d) noexcept
  requires requires { d.head_; }
{
  return (stl2::forward<D>(d).head_);
}

template <std::size_t I, IsData D>
constexpr decltype(auto) raw_get(meta::size_t<I>, D&& d) noexcept
  requires I < remove_reference_t<D>::size &&
    requires { d.tail_; }
{
  return raw_get(meta::size_t<I - 1>{}, stl2::forward<D>(d).tail_);
}

template <class T>
constexpr remove_reference_t<T>&
cook(element_t<meta::id_t<T>>& t) noexcept {
  return t;
}

template <class T>
constexpr const remove_reference_t<T>&
cook(const element_t<meta::id_t<T>>& t) noexcept {
  return t;
}

template <class T>
constexpr T&& cook(element_t<meta::id_t<T>>&& t) noexcept {
  return stl2::move(cook<T>(t));
}

template <std::size_t I, std::size_t End, class F>
[[noreturn]] void
with_static_index(meta::size_t<I>, meta::size_t<End>, std::size_t, F&&) noexcept {
  std::terminate();
}

template <std::size_t I, std::size_t End, class F>
  requires I < End
constexpr void
with_static_index(meta::size_t<I>, meta::size_t<End>, std::size_t n, F&& f) {
  if (n == I) {
    stl2::forward<F>(f)(meta::size_t<I>{});
  } else {
    with_static_index(meta::size_t<I+1>{}, meta::size_t<End>{}, n, stl2::forward<F>(f));
  }
}

template <std::size_t End, class F>
constexpr void with_static_index(meta::size_t<End>, std::size_t n, F&& f) {
  assert(n < End);
  with_static_index(meta::size_t<0>{}, meta::size_t<End>{}, n, stl2::forward<F>(f));
}

class access {
  template <std::size_t I, class T, class V>
    // FIXME: constrain remove_reference_t<V> to instance of base.
  static constexpr decltype(auto) get(V&& v) {
    if (v.index() != I) {
      throw bad_variant_access{};
    }
    return __variant::cook<T>(__variant::raw_get(
      meta::size_t<I>{}, stl2::forward<V>(v).data_
    ));
  }

  template <std::size_t I, class...Types>
  friend constexpr decltype(auto) stl2::v1::get(__variant::base<Types...>& v);

  template <std::size_t I, class...Types>
  friend constexpr decltype(auto) stl2::v1::get(const __variant::base<Types...>& v);

  template <std::size_t I, class...Types>
  friend constexpr decltype(auto) stl2::v1::get(__variant::base<Types...>&& v);
};

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
  static constexpr bool ambiguous = meta::_v<constructible_from_<T, I + 1, Rest...>>;
  static constexpr std::size_t index = I;
};

template <class T, class...Types>
using constructible_from =
  constructible_from_<T, 0, Types...>;

template <class...Ts>
class base {
protected:
  friend class access;

  using types = meta::list<Ts...>;
  using data_t = data<element_t<Ts>...>;
  using index_t =
#if 1
    std::size_t;
#else
    meta::if_c<
      (sizeof...(Ts) < std::numeric_limits<std::uint_fast8_t>::max()),
      std::uint_fast8_t,
      meta::if_c<
        (sizeof...(Ts) < std::numeric_limits<std::uint_fast16_t>::max()),
        std::uint_fast16_t,
        meta::if_c<
          (sizeof...(Ts) < std::numeric_limits<std::uint_fast32_t>::max()),
          std::uint_fast32_t,
          meta::if_c<
            (sizeof...(Ts) < std::numeric_limits<std::uint_fast64_t>::max()),
            std::uint_fast64_t,
            std::size_t>>>>;
#endif
  static_assert(sizeof...(Ts) < std::numeric_limits<index_t>::max());
  static constexpr auto invalid_index = index_t(-1);

  data_t data_;
  index_t index_;

  template <class F>
  struct visitor {
    base& self_;
    F f_;

    constexpr visitor(base& self, F f) : self_(self), f_(f) {}

    template <std::size_t N>
    constexpr void operator()(meta::size_t<N>) {
      static_assert(N < types::size());
      f_(stl2::get<N>(self_));
    }
  };

  template <class F>
  constexpr void dynamic_visit(F&& f) {
    assert(valid());
    with_static_index(meta::size<types>{}, index(),
                      visitor<F>{*this, stl2::forward<F>(f)});
  }

  base(empty_tag) noexcept :
    data_{empty_tag{}}, index_{invalid_index} {}

public:
  base() requires DefaultConstructible<data_t>() :
    index_{0} {}

  template <class T>
    requires !Same<decay_t<T>, base>() &&
      constructible_from<T&&, Ts...>::value &&
      !constructible_from<T&&, Ts...>::ambiguous &&
      Constructible<data_t, emplaced_index_t<constructible_from<T&&, Ts...>::index>, T&&>()
  constexpr base(T&& t)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<constructible_from<T&&, Ts...>::index>, T&&>::value)
    : data_{emplaced_index<constructible_from<T&&, Ts...>::index>, stl2::forward<T>(t)},
      index_{constructible_from<T&&, Ts...>::index} {}

  template <class T>
    requires !Same<decay_t<T>, base>() &&
      constructible_from<T&&, Ts...>::value &&
      !constructible_from<T&&, Ts...>::ambiguous
  constexpr base(T&& t)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<constructible_from<T&&, Ts...>::index>, T&>::value)
    : data_{emplaced_index<constructible_from<T&&, Ts...>::index>, t},
      index_{constructible_from<T&&, Ts...>::index} {}

  template <class T>
    requires !Same<decay_t<T>, base>() &&
      constructible_from<T&&, Ts...>::value &&
      constructible_from<T&&, Ts...>::ambiguous
  base(T&&) = delete;
  // variant construction from type is ambiguous.

  template <std::size_t I, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, Args...>()
  constexpr base(emplaced_index_t<I>, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, Args...>::value)
    : data_{emplaced_index<I>, stl2::forward<Args>(args)...}, index_{I} {}

  template <std::size_t I, class...Args, _Is<is_reference> T = meta::at_c<types, I>>
  constexpr base(emplaced_index_t<I>, meta::id_t<T> t)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, T&>::value)
    : data_{emplaced_index<I>, t}, index_{I} {}

  template <_IsNot<is_reference> T, class...Args, std::size_t I = index_of_type<T, Ts...>>
    requires Constructible<T, Args...>()
  constexpr base(emplaced_type_t<T>, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, Args...>::value)
    : data_{emplaced_index<I>, stl2::forward<Args>(args)...}, index_{I} {}

  template <_Is<is_reference> T, std::size_t I = index_of_type<T, Ts...>>
  constexpr base(emplaced_type_t<T>, meta::id_t<T> t)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, T&>::value)
    : data_{emplaced_index<I>, t}, index_{I} {}

  template <std::size_t I, class E, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, Args...>()
    constexpr base(emplaced_index_t<I>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, std::initializer_list<E>, Args...>::value)
    : data_{emplaced_index<I>, il, stl2::forward<Args>(args)...}, index_{I} {}

  template <_IsNot<is_reference> T, class E, class...Args, std::size_t I = index_of_type<T, Ts...>>
    requires Constructible<T, Args...>()
  constexpr base(emplaced_type_t<T>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, std::initializer_list<E>, Args...>::value)
    : data_{emplaced_index<I>, il, stl2::forward<Args>(args)...}, index_{I} {}

  constexpr index_t index() const noexcept { return index_; }
  constexpr bool valid() const noexcept { return index_ != invalid_index; }
};

struct destroy_fn {
  Destructible{T}
  constexpr void operator()(T& t) const noexcept {
    t.~T();
  }
};
namespace {
  constexpr auto& destroy = detail::static_const<destroy_fn>::value;
}

template <class...Ts>
class destruct_base : public base<Ts...> {
  using base_t = base<Ts...>;
public:
  ~destruct_base() noexcept {
    if (base_t::valid()) {
      base_t::dynamic_visit(destroy);
    }
  }

  using base_t::base_t;
};

template <class...Ts>
  requires is_trivially_destructible<data<element_t<Ts>...>>::value
class destruct_base<Ts...> : public base<Ts...> {
public:
  using base<Ts...>::base;
};

struct construct_fn {
  Constructible{T, ...Args}
  constexpr void operator()(T& t, Args&&...args) const
    noexcept(std::is_nothrow_constructible<T, Args...>::value) {
    ::new(std::addressof(t)) T{std::forward<Args>(args)...};
  }
};
namespace {
  constexpr auto& construct = detail::static_const<construct_fn>::value;
}

template <class...Ts>
class move_base : public destruct_base<Ts...> {
  using base_t = destruct_base<Ts...>;
public:
  using base_t::base_t;

  move_base() = default;
  constexpr move_base(move_base&& that) :
    base_t{empty_tag{}} {
    if (that.valid()) {
      using size = meta::size<typename base_t::types>;
      with_static_index(size{}, that.index(), [this,&that](auto i) {
        construct(raw_get(i, this->data_),
                  stl2::get<i()>(stl2::move(that)));
        this->index_ = i;
      });
    }
  }
};

template <class...Ts>
  requires is_trivially_move_constructible<data<element_t<Ts>...>>::value ||
    !meta::_v<meta::all_of<meta::list<Ts...>, meta::quote_trait<is_move_constructible>>>
class move_base<Ts...> : public destruct_base<Ts...> {
  using base_t = destruct_base<Ts...>;
public:
  using base_t::base_t;
};

template <class...Ts>
class copy_base : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  using base_t::base_t;

  copy_base() = default;
  copy_base(copy_base&&) = default;
  constexpr copy_base(const copy_base& that) :
    base_t{empty_tag{}} {
    if (that.valid()) {
      with_static_index(meta::size_t<sizeof...(Ts)>{}, that.index(),
        [this,&that](auto i) {
          construct(raw_get(i, this->data_), stl2::get<i()>(that));
          this->index_ = i;
        });
    }
  }
};

template <class...Ts>
  requires is_trivially_copy_constructible<data<element_t<Ts>...>>::value ||
    !meta::_v<meta::all_of<meta::list<Ts...>, meta::quote_trait<is_copy_constructible>>>
class copy_base<Ts...> : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  using base_t::base_t;
};
} // namespace __variant

template <class...Ts>
class variant : public __variant::copy_base<Ts...> {
  using __variant::copy_base<Ts...>::copy_base;
};
template <>
class variant<> {
  variant() requires false;
};

template <std::size_t I, class...Types>
constexpr decltype(auto) get(__variant::base<Types...>& v) {
  static_assert(I < sizeof...(Types));
  using T = meta::at_c<meta::list<Types...>, I>;
  return __variant::access::get<I, T>(v);
}

template <std::size_t I, class...Types>
constexpr decltype(auto) get(const __variant::base<Types...>& v) {
  static_assert(I < sizeof...(Types));
  using T = meta::at_c<meta::list<Types...>, I>;
  return __variant::access::get<I, T>(v);
}

template <std::size_t I, class...Types>
constexpr decltype(auto) get(__variant::base<Types...>&& v) {
  static_assert(I < sizeof...(Types));
  using T = meta::at_c<meta::list<Types...>, I>;
  return __variant::access::get<I, T>(stl2::move(v));
}

template <class T, class...Types>
constexpr decltype(auto) get(__variant::base<Types...>& v) {
  return get<__variant::index_of_type<T, Types...>>(v);
}

template <class T, class...Types>
constexpr decltype(auto) get(const __variant::base<Types...>& v) {
  return get<__variant::index_of_type<T, Types...>>(v);
}

template <class T, class...Types>
constexpr decltype(auto) get(__variant::base<Types...>&& v) {
  return get<__variant::index_of_type<T, Types...>>(stl2::move(v));
}
}} // namespace stl2::v1

#endif
