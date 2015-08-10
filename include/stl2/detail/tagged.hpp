#ifndef STL2_DETAIL_TAGGED_HPP
#define STL2_DETAIL_TAGGED_HPP

#include <tuple>
#include <utility>
#include <meta/meta.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// Tagged tuple-like types [taggedtup]
//
namespace stl2 { inline namespace v1 {
template <class Base, class...Tags>
  requires sizeof...(Tags) <= std::tuple_size<Base>::value
struct tagged;
}}

namespace std {
template <class Base, class...Tags>
struct tuple_size<::stl2::tagged<Base, Tags...>>
  : tuple_size<Base> { };

template <size_t N, class Base, class...Tags>
struct tuple_element<N, ::stl2::tagged<Base, Tags...>>
  : tuple_element<N, Base> { };
}

namespace stl2 { inline namespace v1 {
class __getters {
  template <class Base, class...Tags>
    requires sizeof...(Tags) <= std::tuple_size<Base>::value
  friend struct tagged;

  template <class Type, class Indices, class...Tags>
  class collect_;

  template <class Type, std::size_t...Is, class...Tags>
    requires sizeof...(Is) == sizeof...(Tags)
  class collect_<Type, std::index_sequence<Is...>, Tags...>
    : public Tags::template getter<Type, Is>... {
  protected:
    ~collect_() = default;
  };

  template <class Type, class...Tags>
  using collect = collect_<Type, std::index_sequence_for<Tags...>, Tags...>;
};

// tagged
template <class Base, class...Tags>
  requires sizeof...(Tags) <= std::tuple_size<Base>::value
struct tagged
  : Base, __getters::collect<tagged<Base, Tags...>, Tags...> {
  using Base::Base;
  tagged() = default;

  constexpr tagged(Base&& that)
    noexcept(std::is_nothrow_move_constructible<Base>::value)
    : Base(static_cast<Base&&>(that)) {}

  constexpr tagged(const Base& that)
    noexcept(std::is_nothrow_copy_constructible<Base>::value)
    : Base(static_cast<const Base&>(that)) {}

  template <typename Other>
    requires Constructible<Base, Other>()
  constexpr tagged(tagged<Other, Tags...>&& that)
    noexcept(std::is_nothrow_constructible<Base, Other&&>::value)
    : Base(static_cast<Other&&>(that)) {}

  template <typename Other>
    requires Constructible<Base, const Other&>()
  constexpr tagged(tagged<Other, Tags...> const& that)
    noexcept(std::is_nothrow_constructible<Base, const Other&>::value)
    : Base(static_cast<const Other&>(that)) {}

  template <typename Other>
    requires Assignable<Base&, Other>()
  tagged& operator=(tagged<Other, Tags...>&& that)
    noexcept(std::is_nothrow_assignable<Base&, Other&&>::value) {
    static_cast<Base&>(*this) = static_cast<Other&&>(that);
    return *this;
  }

  template <typename Other>
    requires Assignable<Base&, const Other&>()
  tagged& operator=(const tagged<Other, Tags...>& that)
    noexcept(std::is_nothrow_assignable<Base&, const Other&>::value) {
    static_cast<Base&>(*this) = static_cast<const Other&>(that);
    return *this;
  }

  template <class U>
    requires Assignable<Base&, U>() && !Same<std::decay_t<U>, tagged>()
  tagged& operator=(U&& u) &
    noexcept(std::is_nothrow_assignable<Base&, U&&>::value) {
    static_cast<Base&>(*this) = std::forward<U>(u);
    return *this;
  }

  void swap(tagged& that)
    noexcept(ext::is_nothrow_swappable_v<Base&>)
    requires Swappable<Base&>() {
    using stl2::swap;
    swap(static_cast<Base&>(*this), static_cast<Base&>(that));
  }

  friend void swap(tagged& a, tagged& b)
    noexcept(noexcept(a.swap(b)))
    requires Swappable<Base&>() {
    a.swap(b);
  }
};

template <class T>
struct __tag_spec { };
template <class Spec, class Arg>
struct __tag_spec<Spec(Arg)> { using type = Spec; };

template <class T>
struct __tag_elem { };
template <class Spec, class Arg>
struct __tag_elem<Spec(Arg)> { using type = Arg; };

#define STL2_DEFINE_GETTER(name)                                \
namespace tag {                                                 \
  class name {                                                  \
    friend struct stl2::__getters;                              \
    template <class Derived, std::size_t I>                     \
    struct getter {                                             \
      constexpr decltype(auto) name () &                        \
        requires DerivedFrom<Derived, getter>() {               \
        return std::get<I>(static_cast<Derived&>(*this));       \
      }                                                         \
      constexpr decltype(auto) name () &&                       \
        requires DerivedFrom<Derived, getter>() {               \
        return std::get<I>(static_cast<Derived&&>(*this));      \
      }                                                         \
      constexpr decltype(auto) name () const &                  \
        requires DerivedFrom<Derived, getter>() {               \
        return std::get<I>(static_cast<const Derived&>(*this)); \
      }                                                         \
    protected:                                                  \
      ~getter() = default;                                      \
    };                                                          \
  };                                                            \
}

// tag specifiers [algorithm.general]
STL2_DEFINE_GETTER(in)
STL2_DEFINE_GETTER(in1)
STL2_DEFINE_GETTER(in2)
STL2_DEFINE_GETTER(out)
STL2_DEFINE_GETTER(out1)
STL2_DEFINE_GETTER(out2)
STL2_DEFINE_GETTER(fun)
STL2_DEFINE_GETTER(min)
STL2_DEFINE_GETTER(max)
STL2_DEFINE_GETTER(begin)
STL2_DEFINE_GETTER(end)

#undef STL2_DEFINE_GETTER

// tagged_pair
template <class F, class S>
using tagged_pair = tagged<
  std::pair<meta::_t<__tag_elem<F>>, meta::_t<__tag_elem<S>>>,
  meta::_t<__tag_spec<F>>, meta::_t<__tag_spec<S>>>;

// tagged_tuple
template <class...Types>
using tagged_tuple = tagged<
  std::tuple<meta::_t<__tag_elem<Types>>...>,
  meta::_t<__tag_spec<Types>>...>;
}} // namespace stl2::v1

#endif
