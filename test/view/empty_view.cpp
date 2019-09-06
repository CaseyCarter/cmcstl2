// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/empty.hpp>
#include "../simple_test.hpp"
#include "../single_pass_array.hpp"

namespace ranges = __stl2;

int main() {
	using namespace ranges;

	{
		auto rng = views::empty<double>;

		CHECK(ranges::begin(rng) == nullptr);
		CHECK(rng.begin() == nullptr);
		CHECK(ranges::end(rng) == nullptr);
		CHECK(rng.end() == nullptr);
		CHECK(ranges::data(rng) == nullptr);
		CHECK(rng.data() == nullptr);
		CHECK(ranges::size(rng) == 0);
		CHECK(rng.size() == 0);
	}

	{
		CHECK(ranges::begin(views::empty<int>) == nullptr);
		CHECK(ranges::end(views::empty<int>) == nullptr);
		CHECK(ranges::data(views::empty<int>) == nullptr);
		CHECK(ranges::size(views::empty<int>) == 0);
	}

	{
		CHECK(ranges::begin(views::empty<single_pass_array<int, 1>>) == nullptr);
		CHECK(ranges::end(views::empty<single_pass_array<int, 1>>) == nullptr);
		CHECK(ranges::data(views::empty<single_pass_array<int, 1>>) == nullptr);
		CHECK(ranges::size(views::empty<single_pass_array<int, 1>>) == 0);
	}

	return test_result();
}
