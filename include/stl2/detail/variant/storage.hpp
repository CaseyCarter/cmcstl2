#ifndef STL2_DETAIL_VARIANT_STORAGE_HPP
#define STL2_DETAIL_VARIANT_STORAGE_HPP

#include <initializer_list>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

namespace stl2 { inline namespace v1 { namespace __variant {
struct empty_tag {};

template <Destructible...Ts>
class storage;

template <class>
constexpr bool is_storage = false;
template <Destructible...Ts>
constexpr bool is_storage<storage<Ts...>> = true;

template <class T>
concept bool IsStorage = is_storage<decay_t<T>>;

template <>
class storage<> {};

// TODO: Think about binary instead of linear decomposition of the elements, i.e.,
//       a recursive union binary tree instead of a recursive union list. It will
//       add implementation complexity, but may interact positively with inlining.
//       Be careful about instantiating multiple identical empty leaves - they
//       could enlarge an otherwise small variant with padding.
template <class First, class...Rest>
class storage<First, Rest...> {
  using head_t = First;
  using tail_t = storage<Rest...>;

  static constexpr std::size_t size = 1 + sizeof...(Rest);

  union {
    head_t head_;
    tail_t tail_;
  };

public:
  ~storage() {}

  constexpr storage()
    noexcept(is_nothrow_default_constructible<head_t>::value)
    requires DefaultConstructible<head_t>() :
    head_{} {}

  storage(empty_tag) noexcept {}

  template <std::size_t N, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, Args...>()
  constexpr storage(meta::size_t<N>, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, stl2::forward<Args>(args)...} {}

  template <class...Args>
    requires Constructible<First, Args...>()
  constexpr storage(meta::size_t<0>, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, Args...>::value) :
    head_{stl2::forward<Args>(args)...} {}

  template <std::size_t N, class E, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
  constexpr storage(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, il, stl2::forward<Args>(args)...} {}

  template <class E, class...Args>
    requires Constructible<First, std::initializer_list<E>, Args...>()
  constexpr storage(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
    head_{il, stl2::forward<Args>(args)...} {}
};

template <ext::TriviallyDestructible First, ext::TriviallyDestructible...Rest>
class storage<First, Rest...> {
  using head_t = First;
  using tail_t = storage<Rest...>;

  static constexpr std::size_t size = 1 + sizeof...(Rest);

  union {
    head_t head_;
    tail_t tail_;
  };

public:
  constexpr storage()
    noexcept(is_nothrow_default_constructible<head_t>::value)
    requires DefaultConstructible<head_t>() :
    head_{} {}

  storage(empty_tag) noexcept {}

  template <std::size_t N, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, Args...>()
  constexpr storage(meta::size_t<N>, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, stl2::forward<Args>(args)...} {}

  template <class...Args>
    requires Constructible<First, Args...>()
  constexpr storage(meta::size_t<0>, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, Args...>::value) :
    head_{stl2::forward<Args>(args)...} {}

  template <std::size_t N, class E, class...Args>
    requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
  constexpr storage(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
    tail_{meta::size_t<N - 1>{}, il, stl2::forward<Args>(args)...} {}

  template <class E, class...Args>
    requires Constructible<First, std::initializer_list<E>, Args...>()
  constexpr storage(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
    noexcept(is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
    head_{il, stl2::forward<Args>(args)...} {}
};

struct st_access {
  template <IsStorage D>
  static constexpr auto&& raw_get(meta::size_t<0>, D&& d) noexcept {
    return stl2::forward<D>(d).head_;
  }

  template <std::size_t I, IsStorage D>
  static constexpr auto&& raw_get(meta::size_t<I>, D&& d) noexcept {
    static_assert(I < remove_reference_t<D>::size);
    return st_access::raw_get(meta::size_t<I - 1>{}, stl2::forward<D>(d).tail_);
  }
};
}}} // namespace stl2::v1::__variant

#endif
