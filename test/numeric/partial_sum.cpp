// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/numeric/partial_sum.hpp>

#include "../simple_test.hpp"
#include "../test_iterators.hpp"
#include <array>
#include <forward_list>
#include <list>
#include <stl2/detail/iterator/ostream_iterator.hpp>
#include <stl2/detail/iterator/unreachable.hpp>
#include <stl2/functional.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/transform.hpp>
#include <stl2/view/subrange.hpp>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace ranges {
	using namespace __stl2;
	using namespace __stl2::ext;
} // namespace ranges

namespace view = ranges::view;

template<class I, class S1, class O, class S2, class E>
void test_iterator(I first, S1 last, O result, S2 result_last, E const& expected_output) {
	using ranges::partial_sum;
	auto output_range = ranges::subrange{result, result_last};

	{
		auto const actual_result = partial_sum(first, last, result, result_last);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output_range, expected_output);
	}
	{
		auto const actual_result = partial_sum(ranges::subrange{first, last}, output_range);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output_range, expected_output);
	}
}

template<ranges::Range R, ranges::Range O, ranges::Range E>
void test_iterator(R&& r, O&& result, E const& expected_output) {
	test_iterator(input_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(forward_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(bidirectional_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(random_access_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), expected_output);

	test_iterator(forward_iterator(begin(r)), forward_iterator(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)),
		begin(result), end(result), expected_output);
	test_iterator(random_access_iterator(begin(r)), random_access_iterator(end(r)),
		begin(result), end(result), expected_output);
}

template<class I, class S1, class O, class S2, class BOp, class E>
void test_binary_op(I first, S1 last, O result, S2 result_last, BOp const& bop,
	E const& expected_output) {
	using ranges::partial_sum;
	auto output_range = ranges::subrange{result, result_last};

	{
		auto const actual_result = partial_sum(first, last, result, result_last, bop);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output_range, expected_output);
	}
	{
		auto const actual_result = partial_sum(ranges::subrange{first, last}, output_range, bop);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output_range, expected_output);
	}
}

template<ranges::Range R, ranges::Range O, class BOp, ranges::Range E>
void test_binary_op(R&& r, O&& result, BOp const& bop, E const& expected_output) {
	test_binary_op(input_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(forward_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(bidirectional_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(random_access_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, expected_output);

	test_binary_op(forward_iterator(begin(r)), forward_iterator(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)),
		begin(result), end(result), bop, expected_output);
	test_binary_op(random_access_iterator(begin(r)), random_access_iterator(end(r)),
		begin(result), end(result), bop, expected_output);
}

template<class I, class S1, class O, class S2, class BOp, class Proj, class E>
void test_projection(I first, S1 last, O result, S2 result_last, BOp const& bop, Proj const& proj,
	E const& expected_output) {
	using ranges::partial_sum;
	auto output_range = ranges::subrange{result, result_last};

	{
		auto const actual_result = partial_sum(first, last, result, result_last, bop, proj);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output_range, expected_output);
	}
	{
		auto const actual_result = partial_sum(ranges::subrange{first, last}, output_range, bop,
			proj);
		CHECK(actual_result.in == last);
		CHECK(actual_result.out == result_last);
		CHECK_EQUAL(output_range, expected_output);
	}
}

template<ranges::Range R, ranges::Range O, class BOp, class Proj, ranges::Range E>
void test_projection(R&& r, O&& result, BOp const& bop, Proj const& proj, E const& expected_output)
{
	test_projection(input_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(forward_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(bidirectional_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(random_access_iterator(begin(r)), sentinel(end(r)),
		begin(result), end(result), bop, proj, expected_output);

	test_projection(forward_iterator(begin(r)), forward_iterator(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(bidirectional_iterator(begin(r)), bidirectional_iterator(end(r)),
		begin(result), end(result), bop, proj, expected_output);
	test_projection(random_access_iterator(begin(r)), random_access_iterator(end(r)),
		begin(result), end(result), bop, proj, expected_output);
}

int main()
{
	{
		auto ints = view::iota(0, 10);
		auto output = std::vector(ranges::distance(ints), 0);
		auto const expected_output = std::vector{0, 1, 3, 6, 10, 15, 21, 28, 36, 45};
		test_iterator(ints, output, expected_output);
		test_binary_op(ints, output, ranges::ext::plus{}, expected_output);
		test_projection(ints, output, ranges::ext::plus{}, ranges::identity{}, expected_output);
	}
	{
		using namespace std::string_literals;
		auto const lyrics = std::array{"once a"s, " jolly swagman camped "s, "by a"s, " billabong"s};
		auto output = std::list<std::string>(ranges::distance(lyrics));

		using namespace std::string_view_literals;
		auto const expected_output = std::array{
			"once a"sv,
			"once a jolly swagman camped "sv,
			"once a jolly swagman camped by a"sv,
			"once a jolly swagman camped by a billabong"sv
		};

		test_iterator(lyrics, output, expected_output);
		test_binary_op(lyrics, output, ranges::ext::plus{}, expected_output);
		test_projection(lyrics, output, ranges::ext::plus{}, ranges::identity{}, expected_output);
	}
	{
		auto ints = view::iota(0, 10);
		auto output = std::forward_list<int>(ranges::distance(ints), 0);
		auto const expected_output = std::vector{0, -1, -3, -6, -10, -15, -21, -28, -36, -45};
		test_binary_op(ints, output, ranges::ext::minus{}, expected_output);
		test_projection(ints, output, ranges::ext::minus{}, ranges::identity{}, expected_output);
	}
	{
		struct growth_chart {
			int delta;
		};

		auto height_deltas = std::vector<growth_chart>{
			{50},
			{24},
			{11},
			{9},
			{6},
			{8}
		};
		auto height_by_year = std::vector<int>(ranges::distance(height_deltas));
		constexpr auto expected_output = std::array{50, 74, 85, 94, 100, 108};
		test_projection(height_deltas, height_by_year, ranges::ext::plus{}, &growth_chart::delta,
			expected_output);
	}
	{
		using namespace std::string_literals;
		auto input = std::vector{"0"s, "1"s, "2"s, "3"s, "4"s, "5"s, "6"s, "7"s, "8"s, "9"s};
		using namespace std::string_view_literals;
		auto const expected_output = R"(0
		                                01
		                                012
		                                0123
		                                01234
		                                012345
		                                0123456
		                                01234567
		                                012345678
		                                0123456789
		                                )"sv;
		constexpr auto spacing = "\n		                                ";
		auto output = std::ostringstream{};
		auto result = ranges::subrange{
			ranges::ostream_iterator<std::string>{output, spacing},
			ranges::unreachable{}
		};
		using ranges::ext::partial_sum;
		auto actual_result = partial_sum(begin(input), end(input), begin(result), end(result));
		CHECK(actual_result.in == ranges::end(input));
		// cannot test actual_result.out
		CHECK(output.str() == expected_output);
	}

	return ::test_result();
}
