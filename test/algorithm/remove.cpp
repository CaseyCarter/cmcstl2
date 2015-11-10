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

#include <stl2/detail/algorithm/remove.hpp>
#include <memory>
#include <utility>
#include "../simple_test.hpp"
#include "../test_utils.hpp"
#include "../test_iterators.hpp"

namespace stl2 = __stl2;

template <class Iter, class Sent = Iter>
void
test_iter()
{
    int ia[] = {0, 1, 2, 3, 4, 2, 3, 4, 2};
    constexpr unsigned sa = stl2::size(ia);
    Iter r = stl2::remove(Iter(ia), Sent(ia+sa), 2);
    CHECK(base(r) == ia + sa-3);
    CHECK(ia[0] == 0);
    CHECK(ia[1] == 1);
    CHECK(ia[2] == 3);
    CHECK(ia[3] == 4);
    CHECK(ia[4] == 3);
    CHECK(ia[5] == 4);
}

template <class Iter, class Sent = Iter>
void
test_range()
{
    int ia[] = {0, 1, 2, 3, 4, 2, 3, 4, 2};
    constexpr unsigned sa = stl2::size(ia);
    Iter r = stl2::remove(::as_lvalue(stl2::ext::make_range(Iter(ia), Sent(ia+sa))), 2);
    CHECK(base(r) == ia + sa-3);
    CHECK(ia[0] == 0);
    CHECK(ia[1] == 1);
    CHECK(ia[2] == 3);
    CHECK(ia[3] == 4);
    CHECK(ia[4] == 3);
    CHECK(ia[5] == 4);
}

template <class Iter, class Sent = Iter>
void
test_iter_rvalue()
{
    constexpr unsigned sa = 9;
    std::unique_ptr<int> ia[sa];
    ia[0].reset(new int(0));
    ia[1].reset(new int(1));
    ia[3].reset(new int(3));
    ia[4].reset(new int(4));
    ia[6].reset(new int(3));
    ia[7].reset(new int(4));

#if 0 // FIXME: Why is this here?!?
    using UPI = std::unique_ptr<int>;
    using I1 = decltype(Iter(ia));
    using V1 = stl2::value_type_t<I1>;
    static_assert(stl2::models::Same<V1, UPI>);
    using R1 = stl2::reference_t<I1>;
    static_assert(stl2::models::Same<R1, UPI&>);
    using ICR1 = stl2::iter_common_reference_t<I1>;
    static_assert(stl2::models::Same<ICR1, R1>);
    using I2 = const UPI*;
    using V2 = stl2::value_type_t<I2>;
    static_assert(stl2::models::Same<V1, V2>);
    using R2 = stl2::reference_t<I2>;
    static_assert(stl2::models::Same<R2, const UPI&>);
    using ICR2 = stl2::iter_common_reference_t<I2>;
    static_assert(stl2::models::Same<ICR2, R2>);

    static_assert(stl2::models::Relation<stl2::equal_to<>, V1, V2>);

    static_assert(stl2::models::Same<stl2::common_type_t<V1, R2>, UPI>);
    using CTV1R2 = stl2::CommonType<V1, R2>;
    static_assert(stl2::models::Same<CTV1R2, UPI>);
    static_assert(stl2::models::Common<V1, R2>);
    static_assert(stl2::models::EqualityComparable<V1, R2>);
    //static_assert(stl2::models::Relation<stl2::equal_to<>, V1, R2>);

    static_assert(stl2::models::Common<R1, V2>);
    //static_assert(stl2::models::Relation<stl2::equal_to<>, R1, V2>);

    static_assert(stl2::models::Common<R1, R2>);
    //static_assert(stl2::models::Relation<stl2::equal_to<>, R1, R2>);

    static_assert(stl2::models::Common<ICR1, ICR2>);
    //static_assert(stl2::models::Relation<stl2::equal_to<>, ICR1, ICR2>);

    //static_assert(stl2::models::IndirectCallableRelation<stl2::equal_to<>, I1, I2>);
#endif
    Iter r = stl2::remove(Iter(ia), Sent(ia+sa), std::unique_ptr<int>());
    CHECK(base(r) == ia + sa-3);
    CHECK(*ia[0] == 0);
    CHECK(*ia[1] == 1);
    CHECK(*ia[2] == 3);
    CHECK(*ia[3] == 4);
    CHECK(*ia[4] == 3);
    CHECK(*ia[5] == 4);
}

template <class Iter, class Sent = Iter>
void
test_range_rvalue()
{
    constexpr unsigned sa = 9;
    std::unique_ptr<int> ia[sa];
    ia[0].reset(new int(0));
    ia[1].reset(new int(1));
    ia[3].reset(new int(3));
    ia[4].reset(new int(4));
    ia[6].reset(new int(3));
    ia[7].reset(new int(4));
    Iter r = stl2::remove(::as_lvalue(stl2::ext::make_range(Iter(ia), Sent(ia+sa))), std::unique_ptr<int>());
    CHECK(base(r) == ia + sa-3);
    CHECK(*ia[0] == 0);
    CHECK(*ia[1] == 1);
    CHECK(*ia[2] == 3);
    CHECK(*ia[3] == 4);
    CHECK(*ia[4] == 3);
    CHECK(*ia[5] == 4);
}

struct S
{
    int i;
};

int main()
{
    test_iter<forward_iterator<int*> >();
    test_iter<bidirectional_iterator<int*> >();
    test_iter<random_access_iterator<int*> >();
    test_iter<int*>();
    test_iter<forward_iterator<int*>, sentinel<int*>>();
    test_iter<bidirectional_iterator<int*>, sentinel<int*>>();
    test_iter<random_access_iterator<int*>, sentinel<int*>>();

    test_range<forward_iterator<int*> >();
    test_range<bidirectional_iterator<int*> >();
    test_range<random_access_iterator<int*> >();
    test_range<int*>();
    test_range<forward_iterator<int*>, sentinel<int*>>();
    test_range<bidirectional_iterator<int*>, sentinel<int*>>();
    test_range<random_access_iterator<int*>, sentinel<int*>>();

    test_iter_rvalue<forward_iterator<std::unique_ptr<int>*> >();
    test_iter_rvalue<bidirectional_iterator<std::unique_ptr<int>*> >();
    test_iter_rvalue<random_access_iterator<std::unique_ptr<int>*> >();
    test_iter_rvalue<std::unique_ptr<int>*>();
    test_iter_rvalue<forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_iter_rvalue<bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_iter_rvalue<random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();

    test_range_rvalue<forward_iterator<std::unique_ptr<int>*> >();
    test_range_rvalue<bidirectional_iterator<std::unique_ptr<int>*> >();
    test_range_rvalue<random_access_iterator<std::unique_ptr<int>*> >();
    test_range_rvalue<std::unique_ptr<int>*>();
    test_range_rvalue<forward_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_range_rvalue<bidirectional_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();
    test_range_rvalue<random_access_iterator<std::unique_ptr<int>*>, sentinel<std::unique_ptr<int>*>>();

    // Check projection
    S ia[] = {S{0}, S{1}, S{2}, S{3}, S{4}, S{2}, S{3}, S{4}, S{2}};
    constexpr unsigned sa = stl2::size(ia);
    S* r = stl2::remove(ia, 2, &S::i);
    CHECK(r == ia + sa-3);
    CHECK(ia[0].i == 0);
    CHECK(ia[1].i == 1);
    CHECK(ia[2].i == 3);
    CHECK(ia[3].i == 4);
    CHECK(ia[4].i == 3);
    CHECK(ia[5].i == 4);

    // Check rvalue range
    S ia2[] = {S{0}, S{1}, S{2}, S{3}, S{4}, S{2}, S{3}, S{4}, S{2}};
    constexpr unsigned sa2 = stl2::size(ia2);
    auto r2 = stl2::remove(stl2::move(ia2), 2, &S::i);
    CHECK(r2.get_unsafe() == ia2 + sa2-3);
    CHECK(ia2[0].i == 0);
    CHECK(ia2[1].i == 1);
    CHECK(ia2[2].i == 3);
    CHECK(ia2[3].i == 4);
    CHECK(ia2[4].i == 3);
    CHECK(ia2[5].i == 4);

    return ::test_result();
}
