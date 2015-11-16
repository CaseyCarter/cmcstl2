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
#include <stl2/detail/operator_arrow.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

///////////////////////////////////////////////////////////////////////////
// common_iterator [common.iterators]
//
STL2_OPEN_NAMESPACE {
  struct __ci_access {
    template <class CI>
    static constexpr auto&& v(CI&& ci) noexcept {
      return __stl2::forward<CI>(ci).v_;
    }
  };

  template <Iterator I, Sentinel<I> S, ConvertibleTo<I> II, ConvertibleTo<S> SS>
  struct __ci_convert_visitor {
    constexpr auto operator()(const II& i) const
    STL2_NOEXCEPT_RETURN(
      variant<I, S>{emplaced_index<0>, i}
    )
    constexpr auto operator()(const SS& s) const
    STL2_NOEXCEPT_RETURN(
      variant<I, S>{emplaced_index<1>, s}
    )
  };

  template <Iterator I, Sentinel<I> S>
    requires !Same<I, S>()
  class common_iterator {
    friend __ci_access;
    using var_t = variant<I, S>;
    var_t v_;

  public:
    using difference_type = difference_type_t<I>;

    common_iterator() = default;

    STL2_CONSTEXPR_EXT common_iterator(I i)
    noexcept(is_nothrow_constructible<var_t, I>::value)
    : v_{__stl2::move(i)}
    {}

    STL2_CONSTEXPR_EXT common_iterator(S s)
    noexcept(is_nothrow_constructible<var_t, S>::value)
    : v_{__stl2::move(s)} {}

    template <ConvertibleTo<I> II, ConvertibleTo<S> SS>
    STL2_CONSTEXPR_EXT common_iterator(
      const common_iterator<II, SS>& i)
    noexcept(noexcept(
      var_t{__stl2::visit(
        __ci_convert_visitor<I, S, II, SS>{}, __ci_access::v(i))}))
    : v_{__stl2::visit(__ci_convert_visitor<I, S, II, SS>{},
                       __ci_access::v(i))}
    {}

    template <ConvertibleTo<I> II, ConvertibleTo<S> SS>
    STL2_CONSTEXPR_EXT common_iterator& operator=(
      const common_iterator<II, SS >& i) &
    noexcept(noexcept(
      declval<var_t&>() = __stl2::visit(
        __ci_convert_visitor<I, S, II, SS>{}, __ci_access::v(i))))
    {
      v_ = __stl2::visit(__ci_convert_visitor<I, S, II, SS>{},
                         __ci_access::v(i));
      return *this;
    }

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
    STL2_CONSTEXPR_EXT decltype(auto) operator->() const
    noexcept(noexcept(__stl2::__operator_arrow(declval<const I&>())))
    requires Readable<I>()
    {
      STL2_ASSUME(holds_alternative<I>(v_));
      return __stl2::__operator_arrow(__stl2::get_unchecked<I>(v_));
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
             noexcept(++declval<common_iterator&>()))
    {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
  };

  template <InputIterator I, class S>
  struct iterator_category<common_iterator<I, S>> {
    using type = input_iterator_tag;
  };

  template <ForwardIterator I, class S>
  struct iterator_category<common_iterator<I, S>> {
    using type = forward_iterator_tag;
  };

  template <Readable I, class S>
  struct value_type<common_iterator<I, S>> {
    using type = value_type_t<I>;
  };

  template <Iterator I1, Iterator I2, Sentinel<I2> S1, Sentinel<I1> S2>
  struct __ci_equal_visitor {
    constexpr bool operator()(const I1&, const I2&) const noexcept
    {
      return true;
    }
    constexpr bool operator()(const I1& i1, const I2& i2) const
    noexcept(noexcept(bool(i1 == i2)))
    requires EqualityComparable<I1, I2>()
    {
      return i1 == i2;
    }
    constexpr bool operator()(const auto& lhs, const auto& rhs) const
    STL2_NOEXCEPT_RETURN(
      bool(lhs == rhs)
    )
    constexpr bool operator()(const S1&, const S2&) const noexcept {
      return true;
    }
  };

  template <class I1, class I2, Sentinel<I2> S1, Sentinel<I1> S2>
  STL2_CONSTEXPR_EXT bool operator==(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    __stl2::visit(__ci_equal_visitor<I1, I2, S1, S2>{},
                  __ci_access::v(x), __ci_access::v(y))
  )

  template <class I1, class I2, Sentinel<I2> S1, Sentinel<I1> S2>
  STL2_CONSTEXPR_EXT bool operator!=(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    !(x == y)
  )

  template <class I2, SizedSentinel<I2> I1, SizedSentinel<I2> S1, SizedSentinel<I1> S2>
  struct __ci_difference_visitor {
    constexpr difference_type_t<I2> operator()(const auto& lhs, const auto& rhs) const
    STL2_NOEXCEPT_RETURN(
      static_cast<difference_type_t<I2>>(lhs - rhs)
    )
    constexpr difference_type_t<I2> operator()(const S1&, const S2&) const noexcept {
      return 0;
    }
  };

  template <class I2, SizedSentinel<I2> I1, SizedSentinel<I2> S1, SizedSentinel<I1> S2>
  STL2_CONSTEXPR_EXT difference_type_t<I2> operator-(
    const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
  STL2_NOEXCEPT_RETURN(
    __stl2::visit(__ci_difference_visitor<I2, I1, S1, S2>{},
                  __ci_access::v(x), __ci_access::v(y))
  )
} STL2_CLOSE_NAMESPACE

#endif
