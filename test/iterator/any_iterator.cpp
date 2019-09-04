// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/iterator/any_iterator.hpp>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
#include "../simple_test.hpp"

namespace ranges = __stl2;

void test_small() {
	int rg[]{0,1,2,3,4,5,6,7,8,9};
	using AI = ranges::ext::any_input_iterator<int&>;
	AI first{rg};
	AI const last{rg+10};
	auto a1 = first, a2 = a1; (void)a2; // makin' copies
	CHECK(&rg[5] == &*ranges::next(first, 5));
	CHECK(&rg[5] == &(int const&)ranges::iter_move(ranges::next(first, 5)));
	int i = 0;
	for (; first != last; ++first, ++i) {
		CHECK(!(first == last));
		CHECK(*first == rg[i]);
		CHECK(&*first == &rg[i]);
	}
	CHECK(i == 10);
	first = rg;
	CHECK(*first == 0);
}

void test_big() {
	std::stringstream sin{"now is the time for all good personages"};
	using I = std::istream_iterator<std::string>;
	using AI = ranges::ext::any_input_iterator<std::string const &>;
	AI first{I{sin}};
	AI const last{I{}};
	auto a1 = first, a2 = a1; (void)a2; // makin' aliases
	CHECK(first != last);
	if (first != last) {
		CHECK(*first == "now");
		first = ranges::next(first, 7, last);
		CHECK(first != last);
		if (first != last) {
			CHECK(*first == "personages");
			CHECK(++first == last);
		}
	}
}

int main() {
	test_small();
	test_big();
	return ::test_result();
}
