// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/view/istream.hpp>

#include <sstream>
#include <string_view>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

struct moveonly {
	char c;

	moveonly() = default;
	moveonly(moveonly&&) = default;
	moveonly& operator=(moveonly&&) & = default;

	operator char() const { return c; }

	friend std::istream& operator>>(std::istream& is, moveonly& m) {
		is.get(m.c);
		return is;
	}
};

namespace ranges = __stl2;

int main() {
	constexpr std::string_view test = "abcd3210";
	std::istringstream ss{test.data()};
	CHECK_EQUAL(ranges::view::istream<moveonly>(ss), test);
	return ::test_result();
}
