#ifndef STL2_VARIANT_HPP
#define STL2_VARIANT_HPP

#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <meta/meta.hpp>
#include <stl2/functional.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/tuple_like.hpp>
#include <stl2/detail/concepts/object.hpp>

#define __cpp_lib_variant 20150524

namespace stl2 { inline namespace v1 {
template <class...> class variant;

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

class bad_variant_access : public std::logic_error {
public:
  using std::logic_error::logic_error;
  bad_variant_access() :
    std::logic_error{"Attempt to access invalid variant alternative"} {}
};

namespace __variant {
template <class T, class...Types,
    std::size_t I = meta::_v<meta::find_index<meta::list<Types...>, T>>>
  requires I != meta::_v<meta::npos> &&
    Same<meta::find_index<meta::drop_c<meta::list<Types...>, I + 1>, T>, meta::npos>()
constexpr std::size_t index_of_type = I;

template <class...> class base;
}

template <class T, class... Types>
constexpr std::size_t tuple_find<T, variant<Types...>> =
  __variant::index_of_type<T, Types...>;

template <class T, class...Types,
  std::size_t I = __variant::index_of_type<T, Types...>>
constexpr bool holds_alternative(const __variant::base<Types...>& v) noexcept {
  return v.index() == I;
}

namespace __variant {
struct void_storage { void_storage() requires false; };

template <class T>
struct storage_type {
  using type = T;
};
template <_Is<is_reference> T>
struct storage_type<T> {
  using type = reference_wrapper<remove_reference_t<T>>;
};
template <>
struct storage_type<void> {
  using type = void_storage;
};

template <class T>
using storage_t = meta::_t<storage_type<T>>;

struct empty_tag {};

template <class...Ts> // Destructible...Ts
class data;

template <class>
constexpr bool is_data = false;
template <class...Ts>
constexpr bool is_data<data<Ts...>> = true;

template <class T>
concept bool IsData = is_data<decay_t<T>>;

template <>
class data<> {};

// TODO: Think about binary instead of linear decomposition of the elements, i.e.,
//       a recursive union binary tree instead of a recursive union list. It will
//       add implementation complexity, but may interact positively with inlining.
//       Be careful about instantiating multiple identical empty leaves - they
//       could enlarge an otherwise small variant with padding.
template <class First, class...Rest>
class data<First, Rest...> {
public:
  using head_t = First;
  using tail_t = data<Rest...>;

  static constexpr std::size_t size = 1 + sizeof...(Rest);

  union {
    head_t head_;
    tail_t tail_;
  };

  ~data() {}

  constexpr data()
    requires DefaultConstructible<head_t>()
    : head_{} {}

  data(empty_tag) {}

  template <std::size_t N, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, Args...>()
  constexpr data(meta::size_t<N>, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, stl2::forward<Args>(args)...} {}

  template <class...Args>
    requires Constructible<First, Args...>()
  constexpr data(meta::size_t<0>, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, Args...>::value) :
    head_{stl2::forward<Args>(args)...} {}

  template <std::size_t N, class E, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
  constexpr data(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, il, stl2::forward<Args>(args)...} {}

  template <class E, class...Args>
    requires Constructible<First, std::initializer_list<E>, Args...>()
  constexpr data(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
    head_{il, stl2::forward<Args>(args)...} {}
};

template <class First, class...Rest>
  requires _AllAre<is_trivially_destructible, First, Rest...>
class data<First, Rest...> {
public:
  using head_t = First;
  using tail_t = data<Rest...>;

  static constexpr std::size_t size = 1 + sizeof...(Rest);

  union {
    head_t head_;
    tail_t tail_;
  };

  constexpr data()
    requires DefaultConstructible<head_t>()
    : head_{} {}

  data(empty_tag) {}

  template <std::size_t N, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, Args...>()
  constexpr data(meta::size_t<N>, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, stl2::forward<Args>(args)...} {}

  template <class...Args>
    requires Constructible<First, Args...>()
  constexpr data(meta::size_t<0>, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, Args...>::value) :
    head_{stl2::forward<Args>(args)...} {}

  template <std::size_t N, class E, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
  constexpr data(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, il, stl2::forward<Args>(args)...} {}

  template <class E, class...Args>
    requires Constructible<First, std::initializer_list<E>, Args...>()
  constexpr data(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
    head_{il, stl2::forward<Args>(args)...} {}
};

template <IsData D>
constexpr decltype(auto) raw_get(meta::size_t<0>, D&& d) noexcept {
  return (stl2::forward<D>(d).head_);
}

template <std::size_t I, IsData D>
  requires I < remove_reference_t<D>::size
constexpr decltype(auto) raw_get(meta::size_t<I>, D&& d) noexcept {
  return raw_get(meta::size_t<I - 1>{}, stl2::forward<D>(d).tail_);
}

template <_IsNot<is_const> T>
constexpr remove_reference_t<T>&
cook(storage_t<meta::id_t<T>>& t) noexcept {
  return t;
}

template <class T>
constexpr const remove_reference_t<T>&
cook(const storage_t<meta::id_t<T>>& t) noexcept {
  return t;
}

template <class T>
constexpr T&& cook(storage_t<meta::id_t<T>>&& t) noexcept {
  return stl2::move(cook<T>(t));
}

template <class Indices, class F, class I = meta::front<Indices>>
  requires Indices::size() == 1u
constexpr decltype(auto)
with_static_index(Indices, std::size_t n, F&& f)
  noexcept(noexcept(stl2::forward<F>(f)(I{}))) {
  assert(n == meta::_v<I>);
  return stl2::forward<F>(f)(I{});
}

template <class Indices, class F, class I = meta::front<Indices>>
constexpr decltype(auto)
with_static_index(Indices, std::size_t n, F&& f)
  noexcept(noexcept(
    stl2::forward<F>(f)(I{}),
    with_static_index(meta::pop_front<Indices>{}, n, stl2::forward<F>(f))
  ))
{
  assert(n >= meta::_v<I>);
  if (n <= meta::_v<I>) {
    return stl2::forward<F>(f)(I{});
  } else {
    return with_static_index(meta::pop_front<Indices>{}, n, stl2::forward<F>(f));
  }
}

template <class List>
using indices_for = meta::as_list<meta::make_index_sequence<List::size()>>;

template <class Types>
using non_void_types = meta::filter<Types,
    meta::compose<meta::quote<meta::not_>, meta::quote_trait<is_void>>>;

// Convert a list of types into a list of the indices of the non-is_void types.
template <class Types>
using non_void_indices = meta::transform<
  meta::filter<
    meta::zip<meta::list<indices_for<Types>, Types>>,
    meta::compose<
      meta::quote<meta::not_>,
      meta::quote_trait<is_void>,
      meta::quote<meta::second>>>,
  meta::quote<meta::first>>;

template <class Types, class F, class Indices = non_void_indices<Types>>
  requires !meta::_v<meta::empty<Indices>>
constexpr decltype(auto) with_static_index(std::size_t n, F&& f)
  noexcept(noexcept(with_static_index(Indices{}, n, stl2::forward<F>(f)))) {
  return with_static_index(Indices{}, n, stl2::forward<F>(f));
}

class access {
  template <std::size_t I, _IsNot<is_void> T, class V, class R = __uncvref<V>>
    // FIXME: constrain R to be a specialization of base.
    requires I < decltype(R::data_)::size
  static constexpr decltype(auto) get(V&& v) {
    if (v.index() != I) {
      throw bad_variant_access{};
    }
    return cook<T>(raw_get(
      meta::size_t<I>{}, stl2::forward<V>(v).data_
    ));
  }

  template <std::size_t I, class...Types>
    requires I < sizeof...(Types)
  friend constexpr decltype(auto) stl2::v1::get(__variant::base<Types...>& v);

  template <std::size_t I, class...Types>
    requires I < sizeof...(Types)
  friend constexpr decltype(auto) stl2::v1::get(const __variant::base<Types...>& v);

  template <std::size_t I, class...Types>
    requires I < sizeof...(Types)
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
  using data_t = data<storage_t<Ts>...>;
  using index_t =
#if 0
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

  data_t data_;
  index_t index_;

  base(empty_tag) noexcept :
    data_{empty_tag{}}, index_{invalid_index} {}

public:
  constexpr base()
    noexcept(is_nothrow_default_constructible<data_t>::value)
    requires DefaultConstructible<data_t>() :
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
  // variant construction from T is ambiguous.

  template <std::size_t I, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_index_t<I>, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, Args...>::value)
    : data_{emplaced_index<I>, stl2::forward<Args>(args)...}, index_{I} {}

  template <std::size_t I, class...Args, _Is<is_reference> T = meta::at_c<types, I>>
  explicit constexpr base(emplaced_index_t<I>, meta::id_t<T> t)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, T&>::value)
    : data_{emplaced_index<I>, t}, index_{I} {}

  template <_IsNot<is_reference> T, class...Args, std::size_t I = index_of_type<T, Ts...>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_type_t<T>, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, Args...>::value)
    : data_{emplaced_index<I>, stl2::forward<Args>(args)...}, index_{I} {}

  template <_Is<is_reference> T, std::size_t I = index_of_type<T, Ts...>>
  explicit constexpr base(emplaced_type_t<T>, meta::id_t<T> t)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, T&>::value)
    : data_{emplaced_index<I>, t}, index_{I} {}

  template <std::size_t I, class E, class...Args, _IsNot<is_reference> T = meta::at_c<types, I>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_index_t<I>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, std::initializer_list<E>, Args...>::value)
    : data_{emplaced_index<I>, il, stl2::forward<Args>(args)...}, index_{I} {}

  template <_IsNot<is_reference> T, class E, class...Args, std::size_t I = index_of_type<T, Ts...>>
    requires Constructible<T, Args...>()
  explicit constexpr base(emplaced_type_t<T>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<data_t, emplaced_index_t<I>, std::initializer_list<E>, Args...>::value)
    : data_{emplaced_index<I>, il, stl2::forward<Args>(args)...}, index_{I} {}

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
};

template <std::size_t I, class...Types>
  requires I < sizeof...(Types)
constexpr decltype(auto) get(base<Types...>& v) {
  using T = meta::at_c<meta::list<Types...>, I>;
  return access::get<I, T>(v);
}

template <std::size_t I, class...Types>
  requires I < sizeof...(Types)
constexpr decltype(auto) get(const base<Types...>& v) {
  using T = meta::at_c<meta::list<Types...>, I>;
  return access::get<I, T>(v);
}

template <std::size_t I, class...Types>
  requires I < sizeof...(Types)
constexpr decltype(auto) get(base<Types...>&& v) {
  using T = meta::at_c<meta::list<Types...>, I>;
  return access::get<I, T>(stl2::move(v));
}

template <class T, class...Types>
  requires index_of_type<T, Types...> != tuple_not_found
constexpr decltype(auto) get(base<Types...>& v) {
  return get<index_of_type<T, Types...>>(v);
}

template <class T, class...Types>
  requires index_of_type<T, Types...> != tuple_not_found
constexpr decltype(auto) get(const base<Types...>& v) {
  return get<index_of_type<T, Types...>>(v);
}

template <class T, class...Types>
  requires index_of_type<T, Types...> != tuple_not_found
constexpr decltype(auto) get(base<Types...>&& v) {
  return get<index_of_type<T, Types...>>(stl2::move(v));
}

template <class...V>
// FIXME: require uncvref<V> is derived from a specialization of base
static constexpr std::size_t total_alternatives = 1;
template <class First, class...Rest>
static constexpr std::size_t total_alternatives<First, Rest...> =
  non_void_types<typename __uncvref<First>::types>::size() *
  total_alternatives<Rest...>;

template <class F, class V, class I,
  class T = meta::at<typename __uncvref<V>::types, I>>
using visit_handler_return_t =
  decltype(declval<F>()(cook<T>(raw_get(I{}, declval<V>().data_))));

template <class F, class V>
using all_return_types = meta::transform<
  non_void_indices<typename __uncvref<V>::types>,
  meta::bind_front<meta::quote<visit_handler_return_t>, F, V>>;

#if 0
// require visitation to return the same type for all alternatives.
template <class F, class V, class Types = all_return_types<F, V>>
  requires meta::_v<meta::all_of<
    meta::pop_front<Types>,
    meta::bind_front<meta::quote_trait<is_same>, meta::front<Types>>>>
using visit_return_t = meta::front<Types>;
#elif 0
// require the return type of all alternatives to have a common
// type which visit returns.
template <class F, class V>
using visit_return_t =
  meta::apply_list<meta::quote<common_type_t>, all_return_types<F, V>>;
#else
// require the return type of all alternatives to have a common
// reference type which visit returns.
template <class F, class V>
using visit_return_t =
   meta::apply_list<meta::quote<common_reference_t>, all_return_types<F, V>>;
#endif

// TODO: Tune.
constexpr std::size_t O1_visit_threshold = 5;

template <std::size_t N, class F, class V,
  class Types = typename __uncvref<V>::types,
  class T = meta::at_c<Types, N>,
  class C = decltype(cook<T>(raw_get(meta::size_t<N>{}, declval<V>().data_)))>
  requires N < Types::size() &&
    requires (F&& f, C&& c) { ((F&&)f)((C&&)c); }
static constexpr visit_return_t<F, V> visit_handler(F&& f, V&& v)
  noexcept(noexcept(((F&&)f)(declval<C>()))) {
  return stl2::forward<F>(f)(cook<T>(
    raw_get(meta::size_t<N>{}, stl2::forward<V>(v).data_)
  ));
}

template <class F, class V>
struct ON_dispatch {
  F&& f_;
  V&& v_;

  template <std::size_t N>
  constexpr visit_return_t<F, V> operator()(meta::size_t<N>) &&
  noexcept(noexcept(visit_handler<N>(declval<F>(), declval<V>()))) {
    return visit_handler<N>(stl2::forward<F>(f_), stl2::forward<V>(v_));
  }
};

template <class F, class V, class Types = typename __uncvref<V>::types>
  requires total_alternatives<V> < O1_visit_threshold
// FIXME: require uncvref<V> is derived from a specialization of base
constexpr visit_return_t<F, V> visit(F&& f, V&& v)
  noexcept(noexcept(
    with_static_index<Types>(0, declval<ON_dispatch<F, V>>())
  ))
{
  assert(v.index() < Types::size());
  return with_static_index<Types>(
    v.index(), ON_dispatch<F, V>{stl2::forward<F>(f), stl2::forward<V>(v)}
  );
}

template <class F, class V>
using visit_handler_ptr = visit_return_t<F, V>(*)(F&&, V&&);

template <std::size_t I, class F, class V>
constexpr visit_handler_ptr<F, V> visit_handler_for = {};
template <std::size_t I, class F, class V>
  requires _IsNot<meta::at_c<typename __uncvref<V>::types, I>, is_void>
constexpr visit_handler_ptr<F, V> visit_handler_for<I, F, V> =
  &visit_handler<I, F, V>;

template <class F, class V, class Indices>
struct O1_dispatch;
template <class F, class V, std::size_t...Is>
struct O1_dispatch<F, V, std::index_sequence<Is...>> {
  static constexpr visit_handler_ptr<F, V> table[] = {
    visit_handler_for<Is, F, V>...
  };
};

template <class F, class V, std::size_t...Is>
constexpr visit_handler_ptr<F, V>
  O1_dispatch<F, V, std::index_sequence<Is...>>::table[];

template <class F, class V>
  requires total_alternatives<V> >= O1_visit_threshold
// FIXME: require uncvref<V> is derived from a specialization of base
constexpr visit_return_t<F, V> visit(F&& f, V&& v) {
  using Indices = std::make_index_sequence<__uncvref<V>::types::size()>;
  using Dispatch = O1_dispatch<F, V, Indices>;
  assert(v.index() <= meta::_v<extent<decltype(Dispatch::table)>>);
  assert(Dispatch::table[v.index()]);
  return Dispatch::table[v.index()](stl2::forward<F>(f), stl2::forward<V>(v));
}

template <class F, class First, class...Rest>
// FIXME: constraints, constexpr (can't be constexpr with lambdas!)
decltype(auto) visit(F&& fn, First&& first, Rest&&...rest) {
  return visit([&](auto&& f) {
    return visit([&](auto&&...r){
      return stl2::forward<F>(fn)(stl2::forward<decltype(f)>(f),
                                  stl2::forward<decltype(r)>(r)...);
    }, stl2::forward<Rest>(rest)...);
  }, stl2::forward<First>(first));
}

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
protected:
  void clear() {
    if (this->valid()) {
      visit(destroy, *this);
      this->index_ = this->invalid_index;
    }
  }

public:
  ~destruct_base() noexcept {
    clear();
  }

  destruct_base() = default;
  destruct_base(destruct_base&&) = default;
  destruct_base(const destruct_base&) = default;
  destruct_base& operator=(destruct_base&&) & = default;
  destruct_base& operator=(const destruct_base&) & = default;

  using base_t::base_t;
};

template <class...Ts>
  requires is_trivially_destructible<data<storage_t<Ts>...>>::value
class destruct_base<Ts...> : public base<Ts...> {
  using base_t = base<Ts...>;
protected:
  void clear() {}
public:
  using base_t::base_t;
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
  move_base(move_base&&) = delete;
  move_base(const move_base&) = default;
  move_base& operator=(move_base&&) & = default;
  move_base& operator=(const move_base&) = default;
};

template <class...Ts>
  requires _AllAre<is_move_constructible, storage_t<Ts>...>
class move_base<Ts...> : public destruct_base<Ts...> {
  using base_t = destruct_base<Ts...>;
public:
  using base_t::base_t;

  move_base() = default;
  move_base(move_base&& that)
    noexcept(meta::_v<meta::all_of<meta::list<storage_t<Ts>...>,
               meta::quote_trait<is_nothrow_move_constructible>>>) :
    base_t{empty_tag{}} {
    if (that.valid()) {
      with_static_index<typename base_t::types>(that.index(), [this,&that](auto i) {
        construct(raw_get(i, this->data_), raw_get(i, stl2::move(that).data_));
        this->index_ = i;
      });
    }
  }
  move_base(const move_base&) = default;
  move_base& operator=(move_base&&) & = default;
  move_base& operator=(const move_base&) = default;
};

template <class...Ts>
  requires _AllAre<is_move_constructible, storage_t<Ts>...> &&
    meta::_v<is_trivially_move_constructible<data<storage_t<Ts>...>>>
class move_base<Ts...> : public destruct_base<Ts...> {
  using base_t = destruct_base<Ts...>;
public:
  using base_t::base_t;
};

template <class...Ts>
class move_assign_base : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  using base_t::base_t;

  move_assign_base() = default;
  move_assign_base(move_assign_base&&) = default;
  move_assign_base(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&&) & = delete;
  move_assign_base& operator=(const move_assign_base&) & = default;
};

template <class...Ts>
  requires _AllAre<is_move_assignable, storage_t<Ts>...>
class move_assign_base<Ts...> : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  using base_t::base_t;

  move_assign_base() = default;
  move_assign_base(move_assign_base&&) = default;
  move_assign_base(const move_assign_base&) = default;
  move_assign_base& operator=(move_assign_base&& that) &
    noexcept(meta::_v<meta::all_of<meta::list<storage_t<Ts>...>,
               meta::quote_trait<is_nothrow_move_assignable>>>) {
    using types = typename base_t::types;
    auto i = that.index();
    if (this->index() == i) {
      if (this->valid()) {
        with_static_index<types>(i, [this,&that](auto i) {
          raw_get(i, this->data_) = raw_get(i, stl2::move(that).data_);
        });
      }
    } else {
      this->clear();
      if (that.valid()) {
        with_static_index<types>(that.index(), [this,&that](auto i) {
          construct(raw_get(i, this->data_), raw_get(i, stl2::move(that).data_));
        });
      }
    }
    this->index_ = i;
    return *this;
  }
  move_assign_base& operator=(const move_assign_base&) & = default;
};

template <class...Ts>
  requires _AllAre<is_move_assignable, storage_t<Ts>...> &&
    meta::_v<is_trivially_move_assignable<data<storage_t<Ts>...>>>
class move_assign_base<Ts...> : public move_base<Ts...> {
  using base_t = move_base<Ts...>;
public:
  using base_t::base_t;
};

template <class...Ts>
class copy_base : public move_assign_base<Ts...> {
  using base_t = move_assign_base<Ts...>;
public:
  using base_t::base_t;

  copy_base() = default;
  copy_base(copy_base&&) = default;
  copy_base(const copy_base&) = delete;
  copy_base& operator=(copy_base&&) & = default;
  copy_base& operator=(const copy_base&) & = default;
};

template <class...Ts>
  requires _AllAre<is_copy_constructible, storage_t<Ts>...>
class copy_base<Ts...> : public move_assign_base<Ts...> {
  using base_t = move_assign_base<Ts...>;
public:
  using base_t::base_t;

  copy_base() = default;
  copy_base(copy_base&&) = default;
  copy_base(const copy_base& that)
    noexcept(meta::_v<meta::all_of<meta::list<storage_t<Ts>...>,
               meta::quote_trait<is_nothrow_copy_constructible>>>) :
    base_t{empty_tag{}} {
    if (that.valid()) {
      using types = typename base_t::types;
      with_static_index<types>(that.index(), [this,&that](auto i) {
        construct(raw_get(i, this->data_), raw_get(i, that.data_));
        this->index_ = i;
      });
    }
  }
  copy_base& operator=(copy_base&&) & = default;
  copy_base& operator=(const copy_base&) & = default;
};

template <class...Ts>
  requires _AllAre<is_copy_constructible, storage_t<Ts>...> &&
    meta::_v<is_trivially_copy_constructible<data<storage_t<Ts>...>>>
class copy_base<Ts...> : public move_assign_base<Ts...> {
  using base_t = move_assign_base<Ts...>;
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

using __variant::visit;
using __variant::get;

template <class...Ts>
using tagged_variant = tagged<
  variant<meta::_t<__tag_elem<Ts>>...>,
  meta::_t<__tag_spec<Ts>>...>;
}} // namespace stl2::v1

namespace std {
template <class...Types>
struct tuple_size<stl2::variant<Types...>> :
  ::meta::size_t<sizeof...(Types)> {};

template <size_t I, class...Types>
struct tuple_element<I, stl2::variant<Types...>> :
  ::meta::at_c<::meta::list<Types...>, I> {};
}

#endif
