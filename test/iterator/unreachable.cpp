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
#include <stl2/iterator.hpp>
#include <algorithm>
#include "../simple_test.hpp"

namespace ranges = __stl2;

int strlen_test(const char* p) noexcept {
	using C = ranges::common_iterator<const char*, ranges::unreachable>;
	return ranges::distance(C{p}, std::find(C{p}, C{ranges::unreachable{}}, '\0'));
}

int main() {
	// Not yet: static_assert(strlen_test("Hello, world!") == 13);
	CHECK(strlen_test("123This is a test, this is only a test.456") == 42);
	return ::test_result();
}
