#ifndef STL2_DETAIL_TAGGED_HPP
#define STL2_DETAIL_TAGGED_HPP

#include <utility>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/tuple_like.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// Tagged tuple-like types [taggedtup]
//
namespace stl2 { inline namespace v1 {
template <class T>
struct __tag_spec_ {};
template <class Spec, class Arg>
struct __tag_spec_<Spec(Arg)> { using type = Spec; };
template <class T>
using __tag_spec = meta::_t<__tag_spec_<T>>;

template <class T>
struct __tag_elem_ {};
template <class Spec, class Arg>
struct __tag_elem_<Spec(Arg)> { using type = Arg; };
template <class T>
using __tag_elem = meta::_t<__tag_elem_<T>>;

namespace tag { struct __specifier_tag {}; }

template <class T>
concept bool TagSpecifier() {
  return DerivedFrom<T, tag::__specifier_tag>();
}

template <class T>
concept bool __UnaryFunctionType =
  requires {
    typename __tag_spec<T>;
  };

template <class T>
concept bool TaggedType() {
  return __UnaryFunctionType<T> &&
    TagSpecifier<__tag_spec<T>>();
}

template <class Base, TagSpecifier...Tags>
  requires sizeof...(Tags) <= tuple_size<Base>::value
struct tagged;
}}

namespace std {
template <class Base, ::stl2::TagSpecifier...Tags>
struct tuple_size<::stl2::tagged<Base, Tags...>>
  : tuple_size<Base> { };

template <size_t N, class Base, ::stl2::TagSpecifier...Tags>
struct tuple_element<N, ::stl2::tagged<Base, Tags...>>
  : tuple_element<N, Base> { };
}

namespace stl2 { inline namespace v1 {
template <class T, class Base, class...Tags>
constexpr std::size_t tuple_find<T, tagged<Base, Tags...>> = tuple_find<T, Base>;

class __getters {
  template <class Base, TagSpecifier...Tags>
    requires sizeof...(Tags) <= tuple_size<Base>::value
  friend struct tagged;

  template <class Type, class Indices, TagSpecifier...Tags>
  class collect_;

  template <class Type, std::size_t...Is, TagSpecifier...Tags>
    requires sizeof...(Is) == sizeof...(Tags)
  class collect_<Type, std::index_sequence<Is...>, Tags...>
    : public Tags::template getter<Type, Is>... {
  protected:
    ~collect_() = default;
  };

  template <class Type, TagSpecifier...Tags>
  using collect = collect_<Type, std::index_sequence_for<Tags...>, Tags...>;
};

// tagged
template <class Base, TagSpecifier...Tags>
  requires sizeof...(Tags) <= tuple_size<Base>::value
struct tagged
  : Base, __getters::collect<tagged<Base, Tags...>, Tags...> {
  using Base::Base;
  tagged() = default;

  // 20150810: Extension, converting constructor from Base&&
  constexpr tagged(Base&& that)
    noexcept(is_nothrow_move_constructible<Base>::value)
    : Base(static_cast<Base&&>(that)) {}

  // 20150810: Extension, converting constructor from const Base&
  constexpr tagged(const Base& that)
    noexcept(is_nothrow_copy_constructible<Base>::value)
    : Base(static_cast<const Base&>(that)) {}

  // 20150810: Not to spec: constexpr.
  template <class Other>
    requires Constructible<Base, Other>()
  constexpr tagged(tagged<Other, Tags...>&& that)
    noexcept(is_nothrow_constructible<Base, Other&&>::value)
    : Base(static_cast<Other&&>(that)) {}

  // 20150810: Not to spec: constexpr. Extension: conditional noexcept.
  template <class Other>
    requires Constructible<Base, const Other&>()
  constexpr tagged(tagged<Other, Tags...> const& that)
    noexcept(is_nothrow_constructible<Base, const Other&>::value)
    : Base(static_cast<const Other&>(that)) {}

  template <class Other>
    requires Assignable<Base&, Other>()
  tagged& operator=(tagged<Other, Tags...>&& that)
    noexcept(is_nothrow_assignable<Base&, Other&&>::value) {
    static_cast<Base&>(*this) = static_cast<Other&&>(that);
    return *this;
  }

  // 20150810: Extension: conditional noexcept.
  template <class Other>
    requires Assignable<Base&, const Other&>()
  tagged& operator=(const tagged<Other, Tags...>& that)
    noexcept(is_nothrow_assignable<Base&, const Other&>::value) {
    static_cast<Base&>(*this) = static_cast<const Other&>(that);
    return *this;
  }

  template <class U>
    requires Assignable<Base&, U>() && !Same<decay_t<U>, tagged>()
  tagged& operator=(U&& u) &
    noexcept(is_nothrow_assignable<Base&, U&&>::value) {
    static_cast<Base&>(*this) = stl2::forward<U>(u);
    return *this;
  }

  void swap(tagged& that)
    noexcept(is_nothrow_swappable_v<Base&, Base&>)
    requires Swappable<Base&>() {
    stl2::swap(static_cast<Base&>(*this), static_cast<Base&>(that));
  }

  friend void swap(tagged& a, tagged& b)
    noexcept(noexcept(a.swap(b)))
    requires Swappable<Base&>() {
    a.swap(b);
  }

  // 20150819: Extension.
  constexpr Base& base() & { return *this; }
  constexpr const Base& base() const& { return *this; }
  constexpr Base&& base() && { return stl2::move(*this); }
};

#define STL2_DEFINE_GETTER(name)                                        \
namespace tag {                                                         \
 class name : public __specifier_tag {                                  \
    friend __getters;                                                   \
    template <class Derived, std::size_t I>                             \
    struct getter {                                                     \
      constexpr decltype(auto) name () &                                \
        requires DerivedFrom<Derived, getter>() {                       \
        return get<I>(static_cast<Derived&>(*this).base());             \
      }                                                                 \
      constexpr decltype(auto) name () &&                               \
        requires DerivedFrom<Derived, getter>() {                       \
        return get<I>(static_cast<Derived&&>(*this).base());            \
      }                                                                 \
      constexpr decltype(auto) name () const &                          \
        requires DerivedFrom<Derived, getter>() {                       \
        return get<I>(static_cast<const Derived&>(*this).base());       \
      }                                                                 \
    protected:                                                          \
      ~getter() = default;                                              \
    };                                                                  \
  };                                                                    \
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
}} // namespace stl2::v1

#endif
