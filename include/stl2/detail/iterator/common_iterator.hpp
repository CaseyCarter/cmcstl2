// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/variant.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

///////////////////////////////////////////////////////////////////////////
// common_iterator [common.iterators]
//
STL2_OPEN_NAMESPACE {
  template <class S, class I>
  concept bool __WeakSentinel =
    Iterator<I>() && Regular<S>() &&
    ext::WeaklyEqualityComparable<I, S>();

  struct __ci_access {
    template <class T>
    static constexpr auto&& v(T&& ci) noexcept {
      return __stl2::forward<decltype(ci)>(ci).v_;
    }
  };

  template <class I, class S>
  struct __ci_convert_visitor {
    constexpr auto operator()(const ConvertibleTo<I>& i) const
    STL2_NOEXCEPT_RETURN(
      variant<I, S>{emplaced_type<I>, i}
    )
    constexpr auto operator()(const ConvertibleTo<S>& s) const
    STL2_NOEXCEPT_RETURN(
      variant<I, S>{emplaced_type<S>, s}
    )
  };

  template <Iterator I, __WeakSentinel<I> S>
    requires !Same<I, S>()
  class common_iterator {
    friend __ci_access;
    using var_t = variant<I, S>;
    var_t v_;
  public:
    using difference_type = difference_type_t<I>;
    using value_type = value_type_t<I>;
    common_iterator() = default;
    STL2_CONSTEXPR_EXT common_iterator(I i)
      noexcept(is_nothrow_constructible<var_t, I>::value) :
      v_{__stl2::move(i)} {}
    STL2_CONSTEXPR_EXT common_iterator(S s)
      noexcept(is_nothrow_constructible<var_t, S>::value) :
      v_{__stl2::move(s)} {}
    template <ConvertibleTo<I> U, ConvertibleTo<S> V>
    STL2_CONSTEXPR_EXT common_iterator(const common_iterator<U, V>& u)
      noexcept(noexcept(
        var_t{__stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))}
      ));
    template <ConvertibleTo<I> U, ConvertibleTo<S> V>
    STL2_CONSTEXPR_EXT common_iterator& operator=(const common_iterator<U, V>& u) &
      noexcept(noexcept(
        declval<var_t&>() = __stl2::visit(
          __ci_convert_visitor<I, S>{}, __ci_access::v(u))
      ));
    STL2_CONSTEXPR_EXT decltype(auto) operator*()
      noexcept(noexcept(*declval<I&>()))
    {
      STL2_ASSUME(holds_alternative<I>(v_));
      return *__stl2::get_unchecked<I>(v_);
    }
    STL2_CONSTEXPR_EXT decltype(auto) operator*() const
      noexcept(noexcept(*declval<const I&>()))
      requires detail::Dereferenceable<const I>
    {
      STL2_ASSUME(holds_alternative<I>(v_));
      return *__stl2::get_unchecked<I>(v_);
    }
    STL2_CONSTEXPR_EXT common_iterator& operator++()
    noexcept(noexcept(++declval<I&>()))
    {
      STL2_ASSUME(holds_alternative<I>(v_));
      ++__stl2::get_unchecked<I>(v_);
      return *this;
    }
    STL2_CONSTEXPR_EXT common_iterator operator++(int)
      noexcept(is_nothrow_copy_constructible<common_iterator>::value &&
               is_nothrow_move_constructible<common_iterator>::value &&
               noexcept(++declval<common_iterator&>())) {
      common_iterator tmp(*this);
      ++*this;
      return tmp;
    }
  };

  template<InputIterator I, class S>
  struct iterator_category<common_iterator<I, S>> {
    using type = conditional_t<
      models::ForwardIterator<I>,
        forward_iterator_tag,
        input_iterator_tag>;
  };

  template <class I, class S>
  template <ConvertibleTo<I> U, ConvertibleTo<S> V>
  STL2_CONSTEXPR_EXT
  common_iterator<I, S>::common_iterator(const common_iterator<U, V>& u)
    noexcept(noexcept(var_t{
      __stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))})) :
    v_{__stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))} {}

  template <class I, class S>
  template <ConvertibleTo<I> U, ConvertibleTo<S> V>
  STL2_CONSTEXPR_EXT common_iterator<I, S>&
    common_iterator<I, S>::operator=(const common_iterator<U, V>& u) &
      noexcept(noexcept(
        declval<var_t&>() =
          __stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))))
    {
      v_ = __stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u));
      return *this;
    }

  template <class S1, class S2>
  struct __ci_equal_visitor {
    constexpr bool operator()(const auto& lhs, const auto& rhs) const
    STL2_NOEXCEPT_RETURN(
      lhs == rhs
    )
    constexpr bool operator()(const S1&, const S2&) const noexcept {
      return true;
    }
  };

  template <class I1, class S1, class I2, class S2>
    requires EqualityComparable<I1, I2>() &&
      ext::WeaklyEqualityComparable<I1, S2>() &&
      ext::WeaklyEqualityComparable<I2, S1>()
  STL2_CONSTEXPR_EXT bool operator==(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    __stl2::visit(__ci_equal_visitor<S1, S2>{}, __ci_access::v(x), __ci_access::v(y))
  )

  template <class I1, class S1, class I2, class S2>
    requires EqualityComparable<I1, I2>() &&
      ext::WeaklyEqualityComparable<I1, S2>() &&
      ext::WeaklyEqualityComparable<I2, S1>()
  STL2_CONSTEXPR_EXT bool operator!=(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    !(x == y)
  )

  template <class D, class S1, class S2>
  struct __ci_difference_visitor {
    constexpr D operator()(const auto& lhs, const auto& rhs) const
    STL2_NOEXCEPT_RETURN(
      static_cast<D>(lhs - rhs)
    )
    constexpr D operator()(const S1&, const S2&) const noexcept {
      return 0;
    }
  };

  template <WeaklyIncrementable I1, WeaklyIncrementable I2>
  using __common_difference_type_t =
    common_type_t<difference_type_t<I1>, difference_type_t<I2>>;

  template <class, class, class, class>
  constexpr bool __common_sized_iterator_range = false;
  template <class I1, class S1, class I2, class S2>
    requires SizedIteratorRange<I1, I1>() && SizedIteratorRange<I2, I2>() &&
      requires (const I1 i1, const S1 s1, const I2 i2, const S2 s2) {
        { i1 - i2 } -> __common_difference_type_t<I1, I2>;
        { i2 - i1 } -> __common_difference_type_t<I1, I2>;
        { i1 - s2 } -> __common_difference_type_t<I1, I2>;
        { s2 - i1 } -> __common_difference_type_t<I1, I2>;
        { i2 - s1 } -> __common_difference_type_t<I1, I2>;
        { s1 - i2 } -> __common_difference_type_t<I1, I2>;
     }
  constexpr bool __common_sized_iterator_range<I1, S1, I2, S2> = true;

  template <class I1, class S1, class I2, class S2>
    requires __common_sized_iterator_range<I1, S1, I2, S2>
  STL2_CONSTEXPR_EXT __common_difference_type_t<I1, I2> operator-(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    __stl2::visit(__ci_difference_visitor<
                    __common_difference_type_t<I1, I2>, S1, S2>{},
                  __ci_access::v(x), __ci_access::v(y))
  )
} STL2_CLOSE_NAMESPACE

#endif
