// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/memory/uninitialized_move.hpp>
#include <stl2/concepts.hpp>
#include <stl2/detail/algorithm/all_of.hpp>
#include <stl2/detail/algorithm/count_if.hpp>
#include <stl2/detail/algorithm/equal.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/view/repeat.hpp>
#include <stl2/view/take_exactly.hpp>
#include <cstdint>
#include <memory>
#include <vector>
#include "../simple_test.hpp"
#include "common.hpp"

namespace ranges = __stl2;

namespace {
	template <typename T>
	bool empty(const Array<T>& a) {
		return ranges::all_of(a, &T::empty);
	}

	template <typename T>
	requires ranges::is_fundamental<T>::value
	bool empty(const Array<T>&) {
		return true;
	}

	template <ranges::Copyable T>
	void uninitialized_move_test(const Array<T>& control)
	{
		auto independent = make_buffer<T>(control.size());
		auto to_move = control;
		auto test = [&control, &to_move, &independent](const auto& p) {
			CHECK(::empty(to_move));
			CHECK(p.in() == to_move.end());
			CHECK(p.out() == independent.end());

			CHECK(ranges::equal(control.begin(), control.begin() + (p.in() - to_move.begin()),
					independent.begin(), p.out()));
			ranges::destroy(independent.begin(), p.out());
		};

		test(ranges::uninitialized_move(to_move.begin(), to_move.end(), independent.begin()));

		to_move = control; // to_move.begin(), not to_move.cbegin()
		test(ranges::uninitialized_move(to_move.begin(), to_move.end(), independent.cbegin()));

		to_move = control;
		test(ranges::uninitialized_move(to_move, independent.begin()));

		to_move = control;
		test(ranges::uninitialized_move(to_move, independent.cbegin()));

		to_move = control;
		test(ranges::uninitialized_move_n(to_move.begin(), to_move.size(), independent.begin()));

		to_move = control; // to_move.begin(), not to_move.cbegin()
		test(ranges::uninitialized_move_n(to_move.begin(), to_move.size(), independent.cbegin()));
	}

	using Move_only_t = Array<std::unique_ptr<std::string>>;
	void uninitialized_move_test(Move_only_t first)
	{
		auto test = [](const auto& s, const auto& d, const auto& p) {
			CHECK(p.in() == s.end());
			CHECK(p.out() == d.end());
			auto n = ranges::count_if(s.begin(), p.in(), [](const auto& i){ return !i; });
			CHECK(static_cast<std::size_t>(n) == static_cast<std::size_t>(s.size()));
		};

		auto second = make_buffer<Move_only_t::value_type>(first.size());
		test(first, second, ranges::uninitialized_move(first.begin(), first.end(), second.begin()));
		test(second, first, ranges::uninitialized_move(second.begin(), second.end(), first.cbegin()));
		test(first, second, ranges::uninitialized_move(first, second.begin()));
		test(second, first, ranges::uninitialized_move(second, first.cbegin()));
		test(first, second, ranges::uninitialized_move_n(first.begin(), first.size(), second.cbegin()));
		test(second, first, ranges::uninitialized_move_n(second.begin(), second.size(), first.begin()));
	}

	struct S {
		static constexpr int throw_after = 42;
		static int count;

		static void increment() {
			if (++count >= throw_after) {
				throw exception{};
			}
		}

		struct exception {};

		S() = default;
		S(const S&) = default;
		S& operator=(const S&) & = default;
		S(const S&&) { increment(); }
		S& operator=(const S&&) & {
			increment();
			return *this;
		}
	};
	constexpr int S::throw_after;
	int S::count;

	void throw_test() {
		constexpr int n = 2 * S::throw_after;
		auto control = ranges::ext::repeat_view<S>{S{}};
		auto independent = make_buffer<S>(n);
		S::count = 0;
		try {
			ranges::uninitialized_move_n(control.begin(), n, independent.begin());
			CHECK(false);
		} catch(S::exception&) {
			CHECK(S::count == S::throw_after);
		}
		S::count = 0;

		auto control2 = ranges::ext::take_exactly_view<ranges::ext::repeat_view<S>>{
			std::move(control), n
		};
		S::count = 0;
		try {
			ranges::uninitialized_move(control2, independent.begin());
			CHECK(false);
		} catch(S::exception&) {
			CHECK(S::count == S::throw_after);
		}
		S::count = 0;
	}
}

/**
 * Testing framework:
 * - test an array of fundamentals
 * - test an array of standard containers
 * - test an array of non-standard structures
 *
 * - initial array: using the default constructor
 * - second array:  using a non-default constructor
 */
int main()
{
	using Test_type_one = Array<int>;
	using Test_type_two = Array<std::vector<double>>;
	uninitialized_move_test(Test_type_one{});
	uninitialized_move_test(Test_type_two{});
	uninitialized_move_test(Array<Book>{});

	uninitialized_move_test(Test_type_one{0, 1, 2, 3, 4, 5, 6, 7});
	uninitialized_move_test(Test_type_two{{
		{0.0, 0.1, 0.2},
		{1.0, 1.1, 1.2, 1.3, 1.4},
		{2.0, 2.1, 2.2, 2.3},
		{3.01, 3.20, 3.33, 3.4},
		{4.101, 4.102, 4.201, 4.202, 4.311},
		{5.},
		{6.1, 3.02, 6.3, 6.4, 6.5, 6.6, 6.7, 6.8, 6.9},
		std::vector<double>(1 << 12, 7.0)}});

	uninitialized_move_test(Move_only_t{
		std::make_unique<std::string>("0"),
		std::make_unique<std::string>("0"),
		std::make_unique<std::string>("0"),
		std::make_unique<std::string>("0"),
		std::make_unique<std::string>("0"),
		std::make_unique<std::string>("0"),
		std::make_unique<std::string>("0"),
		std::make_unique<std::string>("0")});

	throw_test();

	return ::test_result();
}
