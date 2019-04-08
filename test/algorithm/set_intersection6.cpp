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

#include <array>
#include "set_intersection.hpp"
#include <stl2/detail/algorithm/lexicographical_compare.hpp>

int main()
{
	// Test projections
	{
		const auto in1 = std::array{S{1}, S{2}, S{2}, S{3}, S{3}, S{3}, S{4}, S{4}, S{4}, S{4}};
		const auto in2 = std::array{T{2}, T{4}, T{4}, T{6}};
		auto out = std::array<U, 20>{};
		const auto expected = std::array{2, 4, 4};

		auto result = stl2::set_intersection(in1, in2, stl2::begin(out), stl2::less{}, &S::i, &T::j);
		CHECK((result.out - stl2::begin(out)) == stl2::distance(expected));
		CHECK(!stl2::lexicographical_compare(
			stl2::begin(out), result.out,
			stl2::begin(expected), stl2::end(expected),
			stl2::less{}, &U::k));
	}

	return ::test_result();
}
