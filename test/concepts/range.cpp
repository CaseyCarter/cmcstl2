// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/begin_end.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/concepts.hpp>

namespace ns {
  template <class R>
  using IteratorType = ranges::range_iterator_t<R>;

  template <class R>
  using SentinelType = ranges::range_sentinel_t<R>;

  using ranges::size;
  using ranges::view_base;
  using ranges::begin;
  using ranges::end;
}

namespace models {
  template <class T, class U>
  constexpr bool Same = ranges::Same<T, U>();

  template <class I>
  constexpr bool Iterator = ranges::Iterator<I>();

  template <class S, class I>
  constexpr bool Sentinel = ranges::IteratorRange<I, S>();

  template <class I>
  constexpr bool InputIterator = ranges::InputIterator<I>();

  template <class R>
  constexpr bool Range = ranges::Range<R>();

  template <class R>
  constexpr bool SizedRange = ranges::SizedRange<R>();

  template <class R>
  constexpr bool SizedRangeLike = ranges::SizedRangeLike_<R>();

  template <class R>
  constexpr bool _ContainerLike = ranges::ContainerLike_<R>();

  template <class R>
  constexpr bool View = ranges::View<R>();

  template <class R>
  constexpr bool BoundedRange = ranges::BoundedRange<R>();

  template <class R, class T>
  constexpr bool OutputRange = ranges::OutputRange<R, T>();

  template <class R>
  constexpr bool InputRange = ranges::InputRange<R>();

  template <class R>
  constexpr bool ForwardRange = ranges::ForwardRange<R>();

  template <class R>
  constexpr bool BidirectionalRange = ranges::BidirectionalRange<R>();

  template <class R>
  constexpr bool RandomAccessRange = ranges::RandomAccessRange<R>();
}

template <bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
  CONCEPT_REQUIRES(allow_nonconst)
  int* begin();
  CONCEPT_REQUIRES(allow_nonconst)
  int* end();
  CONCEPT_REQUIRES(allow_const)
  const int* begin() const;
  CONCEPT_REQUIRES(allow_const)
  const int* end() const;

  CONCEPT_REQUIRES(allow_size)
  int size() const;
};

#elif VALIDATE_STL2
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

namespace ns = ::__stl2;

template <bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
  int* begin() requires allow_nonconst;
  int* end() requires allow_nonconst;
  const int* begin() const requires allow_const;
  const int* end() const requires allow_const;

  int size() const requires allow_size;
};
#endif

#include "../simple_test.hpp"
#include <vector>

using mutable_unsized_range =
  arbitrary_range<true, true, false>;

using mutable_only_unsized_range =
  arbitrary_range<true, false, false>;

using immutable_unsized_range =
  arbitrary_range<false, true, false>;

using mutable_sized_range =
  arbitrary_range<true, true, true>;

using mutable_only_sized_range =
  arbitrary_range<true, false, true>;

using immutable_sized_range =
  arbitrary_range<false, true, true>;

// size() launches the missiles.
struct mutable_badsized_range :
  mutable_sized_range {};

struct mutable_only_badsized_range :
  mutable_only_sized_range {};

struct immutable_badsized_range :
  immutable_sized_range {};

#if VALIDATE_STL2
STL2_OPEN_NAMESPACE {
template <>
struct disable_sized_range<mutable_badsized_range> :
  std::true_type {};
template <>
struct disable_sized_range<mutable_only_badsized_range> :
  std::true_type {};
template <>
struct disable_sized_range<immutable_badsized_range> :
  std::true_type {};
} STL2_CLOSE_NAMESPACE

#elif VALIDATE_RANGES
namespace ranges {
template <>
struct is_sized_range<mutable_badsized_range> :
  std::false_type {};
template <>
struct is_sized_range<mutable_only_badsized_range> :
  std::false_type {};
template <>
struct is_sized_range<immutable_badsized_range> :
  std::false_type {};
}
#endif

struct strange_view
{
  std::vector<int>::iterator begin();
  std::vector<int>::const_iterator begin() const;

  std::vector<int>::iterator end();
  std::vector<int>::const_iterator end() const;
};

struct strange_view2 : strange_view, ns::view_base {};
struct strange_view3 : strange_view2 {};

#if VALIDATE_STL2
STL2_OPEN_NAMESPACE {
  template <>
  struct enable_view<strange_view> : std::true_type {};
  template <>
  struct enable_view<strange_view3> : std::false_type {};
} STL2_CLOSE_NAMESPACE

#elif VALIDATE_RANGES
namespace ranges {
  template <>
  struct is_view<strange_view> : std::true_type {};
  template <>
  struct is_view<strange_view3> : std::false_type {};
}
#endif

void ridiculously_exhaustive_range_property_test() {
  CONCEPT_ASSERT(!models::Range<void>);
  CONCEPT_ASSERT(!models::SizedRange<void>);
  CONCEPT_ASSERT(!models::_ContainerLike<void>);
  CONCEPT_ASSERT(!models::View<void>);

  using I = int*;
  using CI = const int*;

  CONCEPT_ASSERT(models::Iterator<I>);
  CONCEPT_ASSERT(models::Iterator<CI>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<int[2]>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<int[2]>, I>);
  CONCEPT_ASSERT(models::Range<int[2]>);
  CONCEPT_ASSERT(models::SizedRange<int[2]>);
  CONCEPT_ASSERT(models::_ContainerLike<int[2]>);
  CONCEPT_ASSERT(!models::View<int[2]>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<int(&)[2]>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<int(&)[2]>, I>);
  CONCEPT_ASSERT(models::Range<int(&)[2]>);
  CONCEPT_ASSERT(models::SizedRange<int(&)[2]>);
  CONCEPT_ASSERT(!models::_ContainerLike<int(&)[2]>);
  CONCEPT_ASSERT(!models::View<int(&)[2]>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const int[2]>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const int[2]>, CI>);
  CONCEPT_ASSERT(models::Range<const int[2]>);
  CONCEPT_ASSERT(models::SizedRange<const int[2]>);
  CONCEPT_ASSERT(!models::_ContainerLike<const int[2]>);
  CONCEPT_ASSERT(!models::View<const int[2]>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const int(&)[2]>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const int(&)[2]>, CI>);
  CONCEPT_ASSERT(models::Range<const int(&)[2]>);
  CONCEPT_ASSERT(models::SizedRange<const int(&)[2]>);
  CONCEPT_ASSERT(!models::_ContainerLike<const int(&)[2]>);
  CONCEPT_ASSERT(!models::View<const int(&)[2]>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_unsized_range>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_unsized_range>, I>);
  CONCEPT_ASSERT(models::Range<mutable_unsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_unsized_range>);
  CONCEPT_ASSERT(models::_ContainerLike<mutable_unsized_range>);
  CONCEPT_ASSERT(!models::View<mutable_unsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_unsized_range&>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_unsized_range&>, I>);
  CONCEPT_ASSERT(models::Range<mutable_unsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_unsized_range&>);
  CONCEPT_ASSERT(models::_ContainerLike<mutable_unsized_range>);
  CONCEPT_ASSERT(!models::View<mutable_unsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const mutable_unsized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const mutable_unsized_range>, CI>);
  CONCEPT_ASSERT(models::Range<const mutable_unsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_unsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_unsized_range>);
  CONCEPT_ASSERT(!models::View<const mutable_unsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const mutable_unsized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const mutable_unsized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<const mutable_unsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_unsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_unsized_range&>);
  CONCEPT_ASSERT(!models::View<const mutable_unsized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_only_unsized_range&>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_only_unsized_range&>, I>);
  CONCEPT_ASSERT(models::Range<mutable_only_unsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_only_unsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_only_unsized_range>);
  CONCEPT_ASSERT(models::View<mutable_only_unsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_only_unsized_range&>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_only_unsized_range&>, I>);
  CONCEPT_ASSERT(models::Range<mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::View<mutable_only_unsized_range&>);

  CONCEPT_ASSERT(!models::Range<const mutable_only_unsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_only_unsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_only_unsized_range>);
  CONCEPT_ASSERT(!models::View<const mutable_only_unsized_range>);

  CONCEPT_ASSERT(!models::Range<const mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::View<const mutable_only_unsized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<immutable_unsized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<immutable_unsized_range>, CI>);
  CONCEPT_ASSERT(models::Range<immutable_unsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<immutable_unsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<immutable_unsized_range>);
  CONCEPT_ASSERT(models::View<immutable_unsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<immutable_unsized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<immutable_unsized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<immutable_unsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<immutable_unsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<immutable_unsized_range&>);
  CONCEPT_ASSERT(!models::View<immutable_unsized_range&>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const immutable_unsized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const immutable_unsized_range>, CI>);
  CONCEPT_ASSERT(models::Range<const immutable_unsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<const immutable_unsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const immutable_unsized_range>);
  CONCEPT_ASSERT(!models::View<const immutable_unsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const immutable_unsized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const immutable_unsized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<const immutable_unsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<const immutable_unsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const immutable_unsized_range&>);
  CONCEPT_ASSERT(!models::View<const immutable_unsized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_sized_range>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_sized_range>, I>);
  CONCEPT_ASSERT(models::Range<mutable_sized_range>);
  CONCEPT_ASSERT(models::SizedRange<mutable_sized_range>);
  CONCEPT_ASSERT(models::_ContainerLike<mutable_sized_range>);
  CONCEPT_ASSERT(!models::View<mutable_sized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_sized_range&>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_sized_range&>, I>);
  CONCEPT_ASSERT(models::Range<mutable_sized_range&>);
  CONCEPT_ASSERT(models::SizedRange<mutable_sized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_sized_range&>);
  CONCEPT_ASSERT(!models::View<mutable_sized_range&>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const mutable_sized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const mutable_sized_range>, CI>);
  CONCEPT_ASSERT(models::Range<const mutable_sized_range>);
  CONCEPT_ASSERT(models::SizedRange<const mutable_sized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_sized_range>);
  CONCEPT_ASSERT(!models::View<const mutable_sized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const mutable_sized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const mutable_sized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<const mutable_sized_range&>);
  CONCEPT_ASSERT(models::SizedRange<const mutable_sized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_sized_range&>);
  CONCEPT_ASSERT(!models::View<const mutable_sized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_only_sized_range>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_only_sized_range>, I>);
  CONCEPT_ASSERT(models::Range<mutable_only_sized_range>);
  CONCEPT_ASSERT(models::SizedRange<mutable_only_sized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_only_sized_range>);
  CONCEPT_ASSERT(models::View<mutable_only_sized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_only_sized_range&>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_only_sized_range&>, I>);
  CONCEPT_ASSERT(models::Range<mutable_only_sized_range&>);
  CONCEPT_ASSERT(models::SizedRange<mutable_only_sized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_only_sized_range&>);
  CONCEPT_ASSERT(!models::View<mutable_only_sized_range&>);

  CONCEPT_ASSERT(!models::Range<const mutable_only_sized_range>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_only_sized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_only_sized_range>);
  CONCEPT_ASSERT(!models::View<const mutable_only_sized_range>);

  CONCEPT_ASSERT(!models::Range<const mutable_only_sized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_only_sized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_only_sized_range&>);
  CONCEPT_ASSERT(!models::View<const mutable_only_sized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<immutable_sized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<immutable_sized_range>, CI>);
  CONCEPT_ASSERT(models::Range<immutable_sized_range>);
  CONCEPT_ASSERT(models::SizedRange<immutable_sized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<immutable_sized_range>);
  CONCEPT_ASSERT(models::View<immutable_sized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<immutable_sized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<immutable_sized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<immutable_sized_range&>);
  CONCEPT_ASSERT(models::SizedRange<immutable_sized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<immutable_sized_range&>);
  CONCEPT_ASSERT(!models::View<immutable_sized_range&>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const immutable_sized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const immutable_sized_range>, CI>);
  CONCEPT_ASSERT(models::Range<const immutable_sized_range>);
  CONCEPT_ASSERT(models::SizedRange<const immutable_sized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const immutable_sized_range>);
  CONCEPT_ASSERT(!models::View<const immutable_sized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const immutable_sized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const immutable_sized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<const immutable_sized_range&>);
  CONCEPT_ASSERT(models::SizedRange<const immutable_sized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const immutable_sized_range&>);
  CONCEPT_ASSERT(!models::View<const immutable_sized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_badsized_range>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_badsized_range>, I>);
  CONCEPT_ASSERT(models::Range<mutable_badsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_badsized_range>);
  CONCEPT_ASSERT(models::_ContainerLike<mutable_badsized_range>);
  CONCEPT_ASSERT(!models::View<mutable_badsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_badsized_range&>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_badsized_range&>, I>);
  CONCEPT_ASSERT(models::Range<mutable_badsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_badsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_badsized_range&>);
  CONCEPT_ASSERT(!models::View<mutable_badsized_range&>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const mutable_badsized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const mutable_badsized_range>, CI>);
  CONCEPT_ASSERT(models::Range<const mutable_badsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_badsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_badsized_range>);
  CONCEPT_ASSERT(!models::View<const mutable_badsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const mutable_badsized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const mutable_badsized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<const mutable_badsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_badsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_badsized_range&>);
  CONCEPT_ASSERT(!models::View<const mutable_badsized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_only_badsized_range>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_only_badsized_range>, I>);
  CONCEPT_ASSERT(models::Range<mutable_only_badsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_only_badsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_only_badsized_range>);
  CONCEPT_ASSERT(models::View<mutable_only_badsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<mutable_only_badsized_range&>, I>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<mutable_only_badsized_range&>, I>);
  CONCEPT_ASSERT(models::Range<mutable_only_badsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<mutable_only_badsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<mutable_only_badsized_range&>);
  CONCEPT_ASSERT(!models::View<mutable_only_badsized_range&>);

  CONCEPT_ASSERT(!models::Range<const mutable_only_badsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_only_badsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_only_badsized_range>);
  CONCEPT_ASSERT(!models::View<const mutable_only_badsized_range>);

  CONCEPT_ASSERT(!models::Range<const mutable_only_badsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<const mutable_only_badsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const mutable_only_badsized_range&>);
  CONCEPT_ASSERT(!models::View<const mutable_only_badsized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<immutable_badsized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<immutable_badsized_range>, CI>);
  CONCEPT_ASSERT(models::Range<immutable_badsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<immutable_badsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<immutable_badsized_range>);
  CONCEPT_ASSERT(models::View<immutable_badsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<immutable_badsized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<immutable_badsized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<immutable_badsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<immutable_badsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<immutable_badsized_range&>);
  CONCEPT_ASSERT(!models::View<immutable_badsized_range&>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const immutable_badsized_range>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const immutable_badsized_range>, CI>);
  CONCEPT_ASSERT(models::Range<const immutable_badsized_range>);
  CONCEPT_ASSERT(!models::SizedRange<const immutable_badsized_range>);
  CONCEPT_ASSERT(!models::_ContainerLike<const immutable_badsized_range>);
  CONCEPT_ASSERT(!models::View<const immutable_badsized_range>);

  CONCEPT_ASSERT(models::Same<ns::IteratorType<const immutable_badsized_range&>, CI>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<const immutable_badsized_range&>, CI>);
  CONCEPT_ASSERT(models::Range<const immutable_badsized_range&>);
  CONCEPT_ASSERT(!models::SizedRange<const immutable_badsized_range&>);
  CONCEPT_ASSERT(!models::_ContainerLike<const immutable_badsized_range&>);
  CONCEPT_ASSERT(!models::View<const immutable_badsized_range&>);


  CONCEPT_ASSERT(models::Same<ns::IteratorType<std::vector<int>>, std::vector<int>::iterator>);
  CONCEPT_ASSERT(models::Same<ns::SentinelType<std::vector<int>>, std::vector<int>::iterator>);
  CONCEPT_ASSERT(models::Range<std::vector<int>>);
  CONCEPT_ASSERT(models::SizedRange<std::vector<int>>);
  CONCEPT_ASSERT(models::_ContainerLike<std::vector<int>>);
  CONCEPT_ASSERT(!models::View<std::vector<int>>);


  CONCEPT_ASSERT(models::Range<strange_view>);
  CONCEPT_ASSERT(models::Range<strange_view&>);
  CONCEPT_ASSERT(models::View<strange_view>);
  CONCEPT_ASSERT(!models::View<strange_view&>);
  CONCEPT_ASSERT(!models::View<const strange_view>);

  CONCEPT_ASSERT(models::Range<strange_view2>);
  CONCEPT_ASSERT(models::Range<strange_view2&>);
  CONCEPT_ASSERT(models::View<strange_view2>);
  CONCEPT_ASSERT(!models::View<strange_view2&>);
  CONCEPT_ASSERT(!models::View<const strange_view2>);

  CONCEPT_ASSERT(models::Range<strange_view3>);
  CONCEPT_ASSERT(models::Range<strange_view3&>);
  CONCEPT_ASSERT(!models::View<strange_view3>);
  CONCEPT_ASSERT(!models::View<strange_view3&>);
  CONCEPT_ASSERT(!models::View<const strange_view3>);

  CONCEPT_ASSERT(models::Range<mutable_only_unsized_range&>);
  CONCEPT_ASSERT(models::Range<mutable_only_unsized_range>);
  CONCEPT_ASSERT(models::View<mutable_only_unsized_range>);
  CONCEPT_ASSERT(!models::View<mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::View<mutable_only_unsized_range&&>);
  CONCEPT_ASSERT(!models::Range<const mutable_only_unsized_range&>);
  CONCEPT_ASSERT(!models::View<const mutable_only_unsized_range&>);
}

#if VALIDATE_RANGES
template <class I, class S,
  CONCEPT_REQUIRES_(models::InputIterator<I> && models::Sentinel<S, I>)>
#elif VALIDATE_STL2
template <ns::InputIterator I, ns::Sentinel<I> S>
#endif
I complicated_algorithm(I i, S s) {
  static constexpr bool output = false;
  if (output) std::cout << '{';
  if (i != s) {
    if (output) std::cout << *i;
    while (++i != s) {
      if (output) std::cout << ", " << *i;
    }
  }
  if (output) std::cout << "}\n";
  return i;
}

#if VALIDATE_RANGES
template <class R, CONCEPT_REQUIRES_(models::Range<R>)>
#elif VALIDATE_STL2
ns::InputRange{R}
#endif
ns::IteratorType<R> complicated_algorithm(R&& r) {
  return complicated_algorithm(ns::begin(r), ns::end(r));
}

template <class T>
struct array_view {
  T* first_;
  std::size_t n_;

  array_view() = default;
  template <std::size_t N>
  array_view(T (&a)[N]) : first_{a}, n_{N} {}

  auto begin() const { return first_; }
  auto end() const { return first_ + n_; }
  auto size() const { return n_; }
};

void complicated_algorithm_test() {
  static int some_ints[] = {2, 3, 5, 7};
  CONCEPT_ASSERT(models::Range<decltype(some_ints)>);
  CONCEPT_ASSERT(models::SizedRange<decltype(some_ints)>);
  CONCEPT_ASSERT(models::_ContainerLike<decltype(some_ints)>);
  CONCEPT_ASSERT(!models::View<decltype(some_ints)>);
  CHECK(complicated_algorithm(some_ints) == ns::end(some_ints));
  CONCEPT_ASSERT(models::Range<array_view<int>>);
  CONCEPT_ASSERT(models::SizedRange<array_view<int>>);
  CONCEPT_ASSERT(!models::_ContainerLike<array_view<int>>);
  CONCEPT_ASSERT(models::View<array_view<int>>);
  CHECK(complicated_algorithm(array_view<int>{some_ints}) == ns::end(some_ints));
}

int main() {
  ridiculously_exhaustive_range_property_test();
  complicated_algorithm_test();

  {
    using T = int[2];
    CONCEPT_ASSERT(models::BoundedRange<T>);
    CONCEPT_ASSERT(models::OutputRange<T, int>);
    CONCEPT_ASSERT(models::InputRange<T>);
    CONCEPT_ASSERT(models::ForwardRange<T>);
    CONCEPT_ASSERT(models::BidirectionalRange<T>);
    CONCEPT_ASSERT(models::RandomAccessRange<T>);
#if VALIDATE_STL2
    CONCEPT_ASSERT(models::ContiguousRange<T>);
#endif
  }

  return ::test_result();
}
