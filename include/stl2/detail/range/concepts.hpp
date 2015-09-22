// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_RANGE_CONCEPTS_HPP
#define STL2_DETAIL_RANGE_CONCEPTS_HPP

#include <initializer_list>

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>

STL2_OPEN_NAMESPACE {
  ///////////////////////////////////////////////////////////////////////////
  // Range [ranges.range]
  //
  template <class T>
  using IteratorType = decltype(__stl2::begin(declval<T&>()));

  template <class T>
  using SentinelType = decltype(__stl2::end(declval<T&>()));

  template <class T>
  concept bool Range() {
    return requires { typename SentinelType<T>; };
  }

  namespace models {
    template <class>
    constexpr bool Range = false;
    __stl2::Range{R}
    constexpr bool Range<R> = true;
  }

  // 20150729: Extension: DifferenceType<Range>.
  template <Range T>
    requires !_Is<__uncvref<T>, is_array> && !detail::MemberDifferenceType<T>
  struct difference_type<T> :
    difference_type<IteratorType<T>> {};

  ///////////////////////////////////////////////////////////////////////////
  // SizedRange [ranges.sized]
  //
  template <class T>
  struct disable_sized_range :
    disable_sized_range<__uncvref<T>> {};

  template <_Unqual T>
  struct disable_sized_range<T> :
    false_type {};

  template <class R>
  constexpr bool __sized_range = false;
  template <class R>
    requires requires (const R& r) {
      STL2_DEDUCTION_CONSTRAINT(__stl2::size(r), Integral);
      STL2_CONVERSION_CONSTRAINT(__stl2::size(r), DifferenceType<IteratorType<R>>);
    }
  constexpr bool __sized_range<R> = true;

  template <class R>
  concept bool SizedRange() {
    return _IsNot<R, disable_sized_range> &&
      Range<R>() &&
      __sized_range<remove_reference_t<R>>;
  }

  namespace models {
    template <class>
    constexpr bool SizedRange = false;
    __stl2::SizedRange{R}
    constexpr bool SizedRange<R> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // View [ranges.view]
  //
  struct view_base {};

  template <class T>
  concept bool _ContainerLike =
    Range<T>() && Range<const T>() &&
    !Same<ReferenceType<IteratorType<T>>,
          ReferenceType<IteratorType<const T>>>();

  namespace models {
    template <class>
    constexpr bool _ContainerLike = false;
    __stl2::_ContainerLike{R}
    constexpr bool _ContainerLike<R> = true;
  }

  template <class T>
  struct enable_view {};

  // TODO: Convert __view_predicate to variable template
  template <class T>
  struct __view_predicate :
    true_type {};

  template <class T>
    requires _Valid<meta::_t, enable_view<T>>
  struct __view_predicate<T> :
    enable_view<T> {};

  // TODO: Be very certain that "!" here works as intended.
  template <_ContainerLike T>
    requires !(DerivedFrom<T, view_base>() ||
               _Valid<meta::_t, enable_view<T>>)
  struct __view_predicate<T> :
     false_type {};

  template <class T>
  concept bool View() {
    return Range<T>() &&
      Semiregular<T>() &&
      __view_predicate<T>::value;
  }

  namespace models {
    template <class>
    constexpr bool View = false;
    __stl2::View{V}
    constexpr bool View<V> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // BoundedRange [ranges.bounded]
  //
  template <class T>
  concept bool BoundedRange() {
    return Range<T>() && Same<IteratorType<T>, SentinelType<T>>();
  }

  namespace models {
    template <class>
    constexpr bool BoundedRange = false;
    __stl2::BoundedRange{R}
    constexpr bool BoundedRange<R> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // OutputRange [ranges.output]
  //
  template <class R, class T>
  concept bool OutputRange() {
    return Range<R>() && OutputIterator<IteratorType<R>, T>();
  }

  namespace models {
    template <class, class>
    constexpr bool OutputRange = false;
    __stl2::OutputRange{R, T}
    constexpr bool OutputRange<R, T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // InputRange [ranges.input]
  //
  template <class T>
  concept bool InputRange() {
    return Range<T>() && InputIterator<IteratorType<T>>();
  }

  namespace models {
    template <class>
    constexpr bool InputRange = false;
    __stl2::InputRange{R}
    constexpr bool InputRange<R> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // ForwardRange [ranges.forward]
  //
  template <class T>
  concept bool ForwardRange() {
    return Range<T>() && ForwardIterator<IteratorType<T>>();
  }

  namespace models {
    template <class>
    constexpr bool ForwardRange = false;
    __stl2::ForwardRange{R}
    constexpr bool ForwardRange<R> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // BidirectionalRange [ranges.bidirectional]
  //
  template <class T>
  concept bool BidirectionalRange() {
    return Range<T>() && BidirectionalIterator<IteratorType<T>>();
  }

  namespace models {
    template <class>
    constexpr bool BidirectionalRange = false;
    __stl2::BidirectionalRange{R}
    constexpr bool BidirectionalRange<R> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // RandomAccessRange [ranges.random.access]
  //
  template <class T>
  concept bool RandomAccessRange() {
    return Range<T>() && RandomAccessIterator<IteratorType<T>>();
  }

  namespace models {
    template <class>
    constexpr bool RandomAccessRange = false;
    __stl2::RandomAccessRange{R}
    constexpr bool RandomAccessRange<R> = true;
  }

  namespace ext {
    template <class R>
    constexpr bool __contiguous_range = false;
    template <class R>
      requires requires (R& r) {
        STL2_EXACT_TYPE_CONSTRAINT(__stl2::data(r), add_pointer_t<ReferenceType<IteratorType<R>>>);
      }
    constexpr bool __contiguous_range<R> = true;

    template <class R>
    concept bool ContiguousRange() {
      return SizedRange<R>() && ContiguousIterator<IteratorType<R>>() &&
        __contiguous_range<remove_reference_t<R>>;
    }
  }

  namespace models {
    template <class>
    constexpr bool ContiguousRange = false;
    __stl2::ext::ContiguousRange{R}
    constexpr bool ContiguousRange<R> = true;
  }
} STL2_CLOSE_NAMESPACE

#endif
