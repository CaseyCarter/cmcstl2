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

  template <class I, class S, class D>
  concept bool __CompatibleSizedIteratorRange =
    SizedIteratorRange<I, S>() &&
    requires (const I i, const S s) {
      {i - s} -> D;
      {s - i} -> D;
    };

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

  template <InputIterator I, __WeakSentinel<I> S>
    requires !Same<I, S>()
  class common_iterator {
    friend __ci_access;
    using var_t = variant<I, S>;
    var_t v_;
  public:
    using difference_type = DifferenceType<I>;
    using value_type = ValueType<I>;
    using iterator_category = conditional_t<
      models::ForwardIterator<I>,
      __stl2::forward_iterator_tag,
      __stl2::input_iterator_tag>;
    using reference = ReferenceType<I>;
    constexpr common_iterator()
      noexcept(is_nothrow_default_constructible<var_t>::value) = default;
    constexpr common_iterator(I i)
      noexcept(is_nothrow_constructible<var_t, I>::value) :
      v_{__stl2::move(i)} {}
    constexpr common_iterator(S s)
      noexcept(is_nothrow_constructible<var_t, S>::value) :
      v_{__stl2::move(s)} {}
    template <ConvertibleTo<I> U, ConvertibleTo<S> V>
    constexpr common_iterator(const common_iterator<U, V>& u)
      noexcept(noexcept(
        var_t{__stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))}
      ));
    template <ConvertibleTo<I> U, ConvertibleTo<S> V>
    constexpr common_iterator& operator=(const common_iterator<U, V>& u) &
      noexcept(noexcept(
        declval<var_t&>() = __stl2::visit(
          __ci_convert_visitor<I, S>{}, __ci_access::v(u))
      ));
    reference operator*() const
      noexcept(noexcept(*declval<const I&>()))
    {
      STL2_ASSERT(holds_alternative<I>(v_));
      return *__stl2::get_unchecked<I>(v_);
    }
    common_iterator& operator++()
    noexcept(noexcept(++declval<I&>()))
    {
      STL2_ASSERT(holds_alternative<I>(v_));
      ++__stl2::get_unchecked<I>(v_);
      return *this;
    }
    common_iterator operator++(int)
      noexcept(is_nothrow_copy_constructible<common_iterator>::value &&
               is_nothrow_move_constructible<common_iterator>::value &&
               noexcept(++declval<common_iterator&>())) {
      common_iterator tmp(*this);
      ++*this;
      return tmp;
    }
  };

  template <class I, class S>
  template <ConvertibleTo<I> U, ConvertibleTo<S> V>
  constexpr common_iterator<I, S>::common_iterator(const common_iterator<U, V>& u)
    noexcept(noexcept(var_t{
      __stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))})) :
    v_{__stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))} {}

  template <class I, class S>
  template <ConvertibleTo<I> U, ConvertibleTo<S> V>
  constexpr common_iterator<I, S>&
    common_iterator<I, S>::operator=(const common_iterator<U, V>& u) &
      noexcept(noexcept(
        declval<var_t&>() =
          __stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u))))
    {
      v_ = __stl2::visit(__ci_convert_visitor<I, S>{}, __ci_access::v(u));
      return *this;
    }

  struct __ci_equal_visitor {
    constexpr bool operator()(const auto& lhs, const auto& rhs) const
    STL2_NOEXCEPT_RETURN(
      lhs == rhs
    )
  };

  template <class I1, class S1, class I2, class S2>
    requires EqualityComparable<I1, I2>() &&
      ext::WeaklyEqualityComparable<I1, S2>() &&
      ext::WeaklyEqualityComparable<I2, S1>()
  constexpr bool operator==(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    __stl2::visit(__ci_equal_visitor{}, __ci_access::v(x), __ci_access::v(y))
  )

  template <class I1, class S1, class I2, class S2>
    requires EqualityComparable<I1, I2>() &&
      ext::WeaklyEqualityComparable<I1, S2>() &&
      ext::WeaklyEqualityComparable<I2, S1>()
  constexpr bool operator!=(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    !(x == y)
  )

  template <class D>
  struct __ci_difference_visitor {
    constexpr D operator()(const auto& lhs, const auto& rhs) const
    STL2_NOEXCEPT_RETURN(
      static_cast<D>(lhs - rhs)
    )
  };

  template <class I1, class S1, class I2, class S2>
    requires
      __CompatibleSizedIteratorRange<I1, I2, DifferenceType<I2>> &&
      __CompatibleSizedIteratorRange<I1, S2, DifferenceType<I2>> &&
      __CompatibleSizedIteratorRange<I2, S1, DifferenceType<I2>>
  constexpr DifferenceType<I2> operator-(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    __stl2::visit(__ci_difference_visitor<DifferenceType<I2>>{},
                  __ci_access::v(x), __ci_access::v(y))
  )

  // Not to spec: extension
  template <class I, class S>
  requires __WeakSentinel<S, I> && !_Valid<__cond, I, S>
  struct common_type<I, S> {
    using type = common_iterator<I, S>;
  };

  template <class S, class I>
    requires __WeakSentinel<S, I> && !_Valid<__cond, I, S>
  struct common_type<S, I> {
    using type = common_iterator<I, S>;
  };
} STL2_CLOSE_NAMESPACE

#endif
