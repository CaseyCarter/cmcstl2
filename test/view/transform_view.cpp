// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/transform.hpp>

#include <memory>
#include <vector>

#include <stl2/detail/algorithm/count.hpp>
#include <stl2/detail/algorithm/transform.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/view/filter.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/reverse.hpp>
#include "../simple_test.hpp"

namespace ranges = __stl2;

namespace {
	struct is_odd {
		bool operator()(int i) const {
			return (i % 2) == 1;
		}
	};
}

int main() {
	using namespace ranges;

	int rgi[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	auto rng = rgi | views::transform(is_odd());
	static_assert(same_as<int &, decltype(*begin(rgi))>);
	static_assert(same_as<bool, decltype(*begin(rng))>);
	static_assert(view<decltype(rng)>);
	static_assert(sized_range<decltype(rng)>);
	static_assert(random_access_range<decltype(rng)>);
	CHECK_EQUAL(rng, {true, false, true, false, true, false, true, false, true, false});

	std::pair<int, int> rgp[] = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}, {7,7}, {8,8}, {9,9}, {10,10}};
	auto rng2 = rgp | views::transform(&std::pair<int,int>::first);
	static_assert(same_as<int &, decltype(*begin(rng2))>);
	static_assert(same_as<iter_value_t<iterator_t<decltype(rng2)>>, int>);
	static_assert(same_as<decltype(iter_move(begin(rng2))), int &&>);
	static_assert(view<decltype(rng2)>);
	static_assert(common_range<decltype(rng2)>);
	static_assert(sized_range<decltype(rng2)>);
	static_assert(random_access_range<decltype(rng2)>);
	CHECK(&*begin(rng2) == &rgp[0].first);
	CHECK(rng2.size() == 10u);
	CHECK_EQUAL(rng2, {1,2,3,4,5,6,7,8,9,10});
	CHECK_EQUAL(rng2 | views::reverse, {10,9,8,7,6,5,4,3,2,1});

	// https://github.com/CaseyCarter/cmcstl2/issues/262
	{
		auto id = [](int x){ return x; };
		views::iota(0) | views::filter(id) | views::transform(id);
	}

	return ::test_result();
}
