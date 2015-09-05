#ifndef STL2_DETAIL_VARIANT_STORAGE_HPP
#define STL2_DETAIL_VARIANT_STORAGE_HPP

#include <initializer_list>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// __variant::storage : Recursive-union implementation.
//
// TODO: Think about binary instead of linear decomposition of the elements,
//       i.e., a recursive union binary tree instead of a recursive union
//       list. It will add implementation complexity, but may interact
//       positively with inlining.
//
STL2_OPEN_NAMESPACE {
  namespace __variant {
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

    ///////////////////////////////////////////////////////////////////////////
    // Specialization for non-trivially destructible element types, has a
    // no-op nontrivial destructor.
    //
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
        tail_{meta::size_t<N - 1>{}, __stl2::forward<Args>(args)...} {}

      template <class...Args>
        requires Constructible<First, Args...>()
      constexpr storage(meta::size_t<0>, Args&&...args)
        noexcept(is_nothrow_constructible<head_t, Args...>::value) :
        head_{__stl2::forward<Args>(args)...} {}

      template <std::size_t N, class E, class...Args>
        requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
      constexpr storage(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
        noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
        tail_{meta::size_t<N - 1>{}, il, __stl2::forward<Args>(args)...} {}

      template <class E, class...Args>
        requires Constructible<First, std::initializer_list<E>, Args...>()
      constexpr storage(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
        noexcept(is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
        head_{il, __stl2::forward<Args>(args)...} {}
    };

    ///////////////////////////////////////////////////////////////////////////
    // Specialization for trivially destructible element types, uses the
    // implicitly-defined (trivial) destructor. Otherwise identical to the
    // other specialization.
    //
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
        tail_{meta::size_t<N - 1>{}, __stl2::forward<Args>(args)...} {}

      template <class...Args>
        requires Constructible<First, Args...>()
      constexpr storage(meta::size_t<0>, Args&&...args)
        noexcept(is_nothrow_constructible<head_t, Args...>::value) :
        head_{__stl2::forward<Args>(args)...} {}

      template <std::size_t N, class E, class...Args>
        requires N > 0 && Constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>()
      constexpr storage(meta::size_t<N>, std::initializer_list<E> il, Args&&...args)
        noexcept(is_nothrow_constructible<tail_t, meta::size_t<N - 1>, std::initializer_list<E>, Args...>::value) :
        tail_{meta::size_t<N - 1>{}, il, __stl2::forward<Args>(args)...} {}

      template <class E, class...Args>
        requires Constructible<First, std::initializer_list<E>, Args...>()
      constexpr storage(meta::size_t<0>, std::initializer_list<E> il, Args&&...args)
        noexcept(is_nothrow_constructible<head_t, std::initializer_list<E>, Args...>::value) :
        head_{il, __stl2::forward<Args>(args)...} {}
    };

    ///////////////////////////////////////////////////////////////////////////
    // st_access::raw_get(meta::size_t<i>, s) returns a "perfect" reference to
    // the element at index i in storage s. The complexity targets of the rest
    // of the variant machinery assume the compiler optimizes the recursion in
    // raw_get to O(1).
    //
    struct st_access {
      template <IsStorage S>
      static constexpr auto&& raw_get(meta::size_t<0>, S&& s) noexcept {
        return __stl2::forward<S>(s).head_;
      }

      template <std::size_t I, IsStorage S>
      static constexpr auto&& raw_get(meta::size_t<I>, S&& s) noexcept {
        static_assert(I < remove_reference_t<S>::size);
        return st_access::raw_get(meta::size_t<I - 1>{}, __stl2::forward<S>(s).tail_);
      }
    };
  }
} STL2_CLOSE_NAMESPACE

#endif
