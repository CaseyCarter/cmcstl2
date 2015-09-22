// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
//  Copyright 2005 - 2007 Adobe Systems Incorporated
//  Distributed under the MIT License(see accompanying file LICENSE_1_0_0.txt
//  or a copy at http://stlab.adobe.com/licenses.html)

//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stl2/detail/algorithm/partition_point.hpp>
#include <stl2/iterator.hpp>
#include <memory>
#include <utility>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace stl2 = __stl2;

struct is_odd
{
    bool operator()(const int& i) const {return i % 2 != 0;}
};

template <class Iter, class Sent = Iter>
void
test_iter()
{
    {
        const int ia[] = {2, 4, 6, 8, 10};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::end(ia)),
                                      is_odd()) == Iter(ia));
    }
    {
        const int ia[] = {1, 2, 4, 6, 8};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::end(ia)),
                                      is_odd()) == Iter(ia + 1));
    }
    {
        const int ia[] = {1, 3, 2, 4, 6};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::end(ia)),
                                      is_odd()) == Iter(ia + 2));
    }
    {
        const int ia[] = {1, 3, 5, 2, 4, 6};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::end(ia)),
                                      is_odd()) == Iter(ia + 3));
    }
    {
        const int ia[] = {1, 3, 5, 7, 2, 4};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::end(ia)),
                                      is_odd()) == Iter(ia + 4));
    }
    {
        const int ia[] = {1, 3, 5, 7, 9, 2};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::end(ia)),
                                      is_odd()) == Iter(ia + 5));
    }
    {
        const int ia[] = {1, 3, 5, 7, 9, 11};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::end(ia)),
                                      is_odd()) == Iter(ia + 6));
    }
    {
        const int ia[] = {1, 3, 5, 2, 4, 6, 7};
        CHECK(stl2::partition_point(Iter(stl2::begin(ia)),
                                      Sent(stl2::begin(ia)),
                                      is_odd()) == Iter(ia));
    }
}

template <class Iter, class Sent = Iter>
void
test_range()
{
    {
        const int ia[] = {2, 4, 6, 8, 10};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::end(ia)))),
                                      is_odd()) == Iter(ia));
    }
    {
        const int ia[] = {1, 2, 4, 6, 8};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::end(ia)))),
                                      is_odd()) == Iter(ia + 1));
    }
    {
        const int ia[] = {1, 3, 2, 4, 6};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::end(ia)))),
                                      is_odd()) == Iter(ia + 2));
    }
    {
        const int ia[] = {1, 3, 5, 2, 4, 6};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::end(ia)))),
                                      is_odd()) == Iter(ia + 3));
    }
    {
        const int ia[] = {1, 3, 5, 7, 2, 4};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::end(ia)))),
                                      is_odd()) == Iter(ia + 4));
    }
    {
        const int ia[] = {1, 3, 5, 7, 9, 2};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::end(ia)))),
                                      is_odd()) == Iter(ia + 5));
    }
    {
        const int ia[] = {1, 3, 5, 7, 9, 11};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::end(ia)))),
                                      is_odd()) == Iter(ia + 6));
    }
    {
        const int ia[] = {1, 3, 5, 2, 4, 6, 7};
        CHECK(stl2::partition_point(::as_lvalue(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                                Sent(stl2::begin(ia)))),
                                      is_odd()) == Iter(ia));
    }

    // An rvalue range
    {
        const int ia[] = {1, 3, 5, 7, 9, 2};
        CHECK(stl2::partition_point(stl2::ext::make_range(Iter(stl2::begin(ia)),
                                                         Sent(stl2::end(ia))),
                                      is_odd()).get_unsafe() == Iter(ia + 5));
    }
}

stl2::WeakIterator{I}
stl2::ext::range<stl2::counted_iterator<I>, stl2::default_sentinel>
make_counted_view(I i, stl2::DifferenceType<I> n) {
  return {stl2::make_counted_iterator(stl2::move(i), n), {}};
}

template <class Iter>
void
test_counted()
{
    {
        const int ia[] = {2, 4, 6, 8, 10};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        stl2::size(ia))),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia), stl2::size(ia)));
    }
    {
        const int ia[] = {1, 2, 4, 6, 8};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        stl2::size(ia))),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia + 1), stl2::size(ia) - 1));
    }
    {
        const int ia[] = {1, 3, 2, 4, 6};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        stl2::size(ia))),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia + 2), stl2::size(ia) - 2));
    }
    {
        const int ia[] = {1, 3, 5, 2, 4, 6};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        stl2::size(ia))),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia + 3), stl2::size(ia) - 3));
    }
    {
        const int ia[] = {1, 3, 5, 7, 2, 4};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        stl2::size(ia))),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia + 4), stl2::size(ia) - 4));
    }
    {
        const int ia[] = {1, 3, 5, 7, 9, 2};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        stl2::size(ia))),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia + 5), stl2::size(ia) - 5));
    }
    {
        const int ia[] = {1, 3, 5, 7, 9, 11};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        stl2::size(ia))),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia + 6), stl2::size(ia) - 6));
    }
    {
        const int ia[] = {1, 3, 5, 2, 4, 6, 7};
        CHECK(stl2::partition_point(::as_lvalue(make_counted_view(Iter(stl2::begin(ia)),
                                                                        0)),
                                      is_odd()) == stl2::counted_iterator<Iter>(Iter(ia), 0));
    }
}

struct S
{
    int i;
};

stl2::Integral{I}
class iota_view {
  I first_;
public:
  class iterator {
    I value_;
  public:
    using value_type = I;
    using difference_type = stl2::make_signed_t<I>;
    using iterator_category = stl2::random_access_iterator_tag;

    iterator() = default;
    constexpr iterator(const iota_view& v) :
      value_{v.first_} {}

    constexpr I operator*() const { return value_; }

    constexpr iterator& operator++() & { ++value_; return *this; }
    constexpr iterator& operator--() & { --value_; return *this; }

    constexpr iterator operator++(int) & {
      auto tmp = *this;
      ++*this;
      return tmp;
    }
    constexpr iterator operator--(int) & {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    constexpr I operator[](difference_type n) const {
      return value_ + n;
    }

    constexpr iterator& operator+=(difference_type n) & {
      value_ += n;
      return *this;
    }
    constexpr iterator& operator-=(difference_type n) & {
      value_ -= n;
      return *this;
    }

    constexpr iterator operator+(difference_type n) const {
      return {value_ + n};
    }
    constexpr iterator operator-(difference_type n) const {
      return {value_ - n};
    }
    friend constexpr iterator operator+(difference_type n, const iterator& i) {
      return i + n;
    }

    friend constexpr difference_type operator-(const iterator& lhs, const iterator& rhs) {
      return lhs.value_ - rhs.value_;
    }

    friend constexpr bool operator==(const iterator& lhs, const iterator& rhs) {
      return lhs.value_ == rhs.value_;
    }
    friend constexpr bool operator!=(const iterator& lhs, const iterator& rhs) {
      return lhs.value_ != rhs.value_;
    }
    friend constexpr bool operator<(const iterator& lhs, const iterator& rhs) {
      return lhs.value_ < rhs.value_;
    }
    friend constexpr bool operator>(const iterator& lhs, const iterator& rhs) {
      return lhs.value_ > rhs.value_;
    }
    friend constexpr bool operator<=(const iterator& lhs, const iterator& rhs) {
      return lhs.value_ <= rhs.value_;
    }
    friend constexpr bool operator>=(const iterator& lhs, const iterator& rhs) {
      return lhs.value_ >= rhs.value_;
    }
  };

  constexpr iota_view(I first = 0) noexcept : first_{first} {}

  constexpr iterator begin() const noexcept { return {*this}; }
  constexpr stl2::unreachable end() const noexcept { return {}; }
};

int main()
{
    test_iter<forward_iterator<const int*> >();
    test_iter<forward_iterator<const int*>, sentinel<const int*>>();

    test_range<forward_iterator<const int*> >();
    test_range<forward_iterator<const int*>, sentinel<const int*>>();

    test_counted<forward_iterator<const int*> >();

    // Test projections
    const S ia[] = {S{1}, S{3}, S{5}, S{2}, S{4}, S{6}};
    CHECK(stl2::partition_point(ia, is_odd(), &S::i) == ia + 3);

    // Test infinite range
    CHECK(*stl2::partition_point(iota_view<int>{0},
                                [](int i){ return i < 42; }).get_unsafe() == 42);

    return ::test_result();
}
