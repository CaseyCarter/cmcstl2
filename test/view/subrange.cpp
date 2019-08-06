// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <list>
#include <vector>
#include <stl2/view/all.hpp>
#include <stl2/view/subrange.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

namespace ranges = __stl2;

using namespace ranges;

template<range Rng>
safe_subrange_t<Rng> algorithm(Rng &&rng);

struct Base {};
struct Derived : Base {};

int main() {
	std::vector<int> vi{1,2,3,4};

	////////////////////////////////////////////////////////////////////////////
	// safe_subrange_t tests:

	// lvalues are ReferenceableRanges and do not dangle:
	static_assert(same_as<subrange<int*>,
		decltype(::algorithm(std::declval<int(&)[42]>()))>);
	static_assert(same_as<subrange<std::vector<int>::iterator>,
		decltype(::algorithm(vi))>);

	// subrange and ref_view are ReferenceableRanges and do not dangle:
	static_assert(same_as<subrange<int*>,
		decltype(::algorithm(std::declval<subrange<int*>>()))>);
	static_assert(same_as<subrange<int*>,
		decltype(::algorithm(std::declval<ref_view<int[42]>>()))>);

	// non-ReferenceableRange rvalue ranges dangle:
	static_assert(same_as<dangling,
		decltype(::algorithm(std::declval<std::vector<int>>()))>);
	static_assert(same_as<dangling,
		decltype(::algorithm(std::move(vi)))>);

	// Test that slicing conversions are not allowed.
	static_assert(constructible_from<subrange<Base*, Base*>, Base*, Base*>);
	static_assert(!constructible_from<subrange<Base*, Base*>, Derived*, Derived*>);
	static_assert(constructible_from<subrange<const Base*, const Base*>, Base*, Base*>);
	static_assert(!constructible_from<subrange<const Base*, const Base*>, Derived*, Derived*>);
	static_assert(!constructible_from<subrange<Base*, Base*>, subrange<Derived*, Derived*>>);

	static_assert(constructible_from<subrange<Base*, unreachable>, Base*, unreachable>);
	static_assert(!constructible_from<subrange<Base*, unreachable>, Derived*, unreachable>);
	static_assert(constructible_from<subrange<const Base*, unreachable>, Base*, unreachable>);
	static_assert(!constructible_from<subrange<const Base*, unreachable>, Derived*, unreachable>);
	static_assert(!constructible_from<subrange<Base*, unreachable>, subrange<Derived*, unreachable>>);

	static_assert(constructible_from<subrange<Base*, unreachable, subrange_kind::sized>, Base*, unreachable, std::ptrdiff_t>);
	static_assert(!constructible_from<subrange<Base*, unreachable, subrange_kind::sized>, Derived*, unreachable, std::ptrdiff_t>);
	static_assert(constructible_from<subrange<const Base*, unreachable, subrange_kind::sized>, Base*, unreachable, std::ptrdiff_t>);
	static_assert(!constructible_from<subrange<const Base*, unreachable, subrange_kind::sized>, Derived*, unreachable, std::ptrdiff_t>);
	static_assert(!constructible_from<subrange<Base*, unreachable, subrange_kind::sized>, subrange<Derived*, unreachable>, std::ptrdiff_t>);

	static_assert(convertible_to<subrange<Base*, Base*>, std::pair<const Base*, const Base*>>);
	static_assert(!convertible_to<subrange<Derived*, Derived*>, std::pair<Base*, Base*>>);

	subrange<std::vector<int>::iterator> r0 {vi.begin(), vi.end()};
	static_assert(std::tuple_size<decltype(r0)>::value == 2);
	static_assert(same_as<std::vector<int>::iterator,
		std::tuple_element<0, decltype(r0)>::type>);
	static_assert(same_as<std::vector<int>::iterator,
		std::tuple_element<1, decltype(r0)>::type>);
	static_assert(sized_range<decltype(r0)>);
	CHECK(r0.size() == 4);
	CHECK(r0.begin() == vi.begin());
	CHECK(get<0>(r0) == vi.begin());
	CHECK(r0.end() == vi.end());
	CHECK(get<1>(r0) == vi.end());
	r0 = r0.next();
	CHECK(r0.size() == 3);

	{
		subrange<std::vector<int>::iterator> rng {vi.begin(), vi.end(), ranges::distance(vi)};
		CHECK(rng.size() == 4);
		CHECK(rng.begin() == vi.begin());
		CHECK(rng.end() == vi.end());
	}

	std::pair<std::vector<int>::iterator, std::vector<int>::iterator> p0 = r0;
	CHECK(p0.first == vi.begin()+1);
	CHECK(p0.second == vi.end());

	subrange<std::vector<int>::iterator, unreachable> r1 { r0.begin(), {} };
	static_assert(std::tuple_size<decltype(r1)>::value == 2);
	static_assert(same_as<std::vector<int>::iterator,
		std::tuple_element<0, decltype(r1)>::type>);
	static_assert(same_as<unreachable,
		std::tuple_element<1, decltype(r1)>::type>);
	static_assert(view<decltype(r1)>);
	static_assert(!sized_range<decltype(r1)>);
	CHECK(r1.begin() == vi.begin()+1);
	r1.end() = unreachable{};

	r0 = r0.next();
	++r0.begin();
	CHECK(r0.begin() == vi.begin()+2);
	CHECK(r0.size() == 2);
	r0 = {r0.begin(), --r0.end()}; // --r0.end();
	CHECK(r0.end() == vi.end()-1);
	CHECK(r0.size() == 1);
	CHECK(r0.front() == 3);
	CHECK(r0.back() == 3);

	std::pair<std::vector<int>::iterator, unreachable> p1 = r1;
	CHECK(p1.first == vi.begin()+1);

	std::list<int> li{1,2,3,4};
	ext::sized_subrange<std::list<int>::iterator> l0 {li.begin(), li.end(),
		static_cast<std::ptrdiff_t>(li.size())};
	static_assert(view<decltype(l0)> && sized_range<decltype(l0)>);
	CHECK(l0.begin() == li.begin());
	CHECK(l0.end() == li.end());
	CHECK(l0.size() == static_cast<std::ptrdiff_t>(li.size()));
	l0 = l0.next();
	CHECK(l0.begin() == next(li.begin()));
	CHECK(l0.end() == li.end());
	CHECK(l0.size() == static_cast<std::ptrdiff_t>(li.size()) - 1);

	l0 = views::all(li);

	subrange<std::list<int>::iterator> l1 = l0;
	static_assert(!sized_range<decltype(l1)>);
	CHECK(l1.begin() == li.begin());
	CHECK(l1.end() == li.end());

	{
		subrange s0{vi.begin(), vi.end()};
		subrange s1{li.begin(), li.end()};
		static_assert(same_as<decltype(r0), decltype(s0)>);
		static_assert(same_as<decltype(l1), decltype(s1)>);
	}
	{
		subrange s0{vi.begin(), vi.end(), ranges::distance(vi)};
		subrange s1{li.begin(), li.end(), ranges::distance(li)};
		static_assert(same_as<decltype(r0), decltype(s0)>);
		static_assert(same_as<decltype(l0), decltype(s1)>);
	}
	{
		subrange s0{vi};
		subrange s1{li};
		subrange s2{views::all(vi)};
		subrange s3{views::all(li)};
		static_assert(same_as<decltype(r0), decltype(s0)>);
		static_assert(same_as<decltype(l0), decltype(s1)>);
		static_assert(same_as<decltype(r0), decltype(s2)>);
		static_assert(same_as<decltype(l0), decltype(s3)>);
	}
	{
		subrange s0{r0};
		subrange s1{l0};
		subrange s2{l1};
		static_assert(same_as<decltype(r0), decltype(s0)>);
		static_assert(same_as<decltype(l0), decltype(s1)>);
		static_assert(same_as<decltype(l1), decltype(s2)>);
	}
	{
		subrange s0{vi, ranges::distance(vi)};
		subrange s1{li, ranges::distance(li)};
		subrange s2{views::all(vi), ranges::distance(vi)};
		subrange s3{views::all(li), ranges::distance(li)};
		static_assert(same_as<decltype(r0), decltype(s0)>);
		static_assert(same_as<decltype(l0), decltype(s1)>);
		static_assert(same_as<decltype(r0), decltype(s2)>);
		static_assert(same_as<decltype(l0), decltype(s3)>);
	}
	{
		subrange s0{r0, size(r0)};
		subrange s1{l0, size(l0)};
		subrange s2{l1, size(l0)};
		static_assert(same_as<decltype(r0), decltype(s0)>);
		static_assert(same_as<decltype(l0), decltype(s1)>);
		static_assert(same_as<decltype(l0), decltype(s2)>);
	}

	return ::test_result();
}
