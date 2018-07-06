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

#include <stl2/view/zip.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/algorithm/sort.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/functional/comparisons.hpp>
#include <stl2/view/common.hpp>
#include <stl2/view/istream.hpp>
#include <stl2/view/transform.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include "../simple_test.hpp"

// TODO:
// * output range tests

template<class...> class show_type; // FIXME: remove

namespace ranges = __stl2;

namespace range_v3_tests {
	using namespace ranges;

	template<class T>
	using range_value_type_t = value_type_t<iterator_t<T>>;
	template<class T>
	using range_reference_t = reference_t<iterator_t<T>>;
	template<class T>
	using range_rvalue_reference_t = rvalue_reference_t<iterator_t<T>>;

	struct MoveOnlyString : std::string {
		using std::string::string;

		MoveOnlyString() = default;
		MoveOnlyString(MoveOnlyString&& that)
		: std::string{static_cast<std::string&&>(that)}
		{ that.clear(); }
		MoveOnlyString& operator=(MoveOnlyString&& that) {
			static_cast<std::string&>(*this) =
				static_cast<std::string&&>(that);
			that.clear();
			return *this;
		}
	};

	void test() {
		std::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		std::vector<std::string> const vs{"hello", "goodbye", "hello", "goodbye"};

		// All common ranges, but one single-pass
		{
			std::stringstream str{"john paul george ringo"};
			using V = std::tuple<int, std::string, std::string>;
			auto rng = view::zip(vi, vs, view::istream<std::string>(str) | view::common);
			using Rng = decltype(rng);
			static_assert(View<Rng>);
			static_assert(InputRange<Rng>);
			static_assert(!ForwardRange<Rng>);
			static_assert(!CommonRange<Rng>);
			static_assert(!SizedRange<Rng>);
			static_assert(Same<
				range_value_type_t<Rng>,
				std::tuple<int, std::string, std::string>>);
			static_assert(Same<
				range_reference_t<Rng>,
				__tuple_hack<int &, std::string const &, std::string &>>);
			static_assert(Same<
				range_rvalue_reference_t<Rng>,
				__tuple_hack<int &&, std::string const &&, std::string &&>>);
			static_assert(ConvertibleTo<range_value_type_t<Rng> &&,
				range_rvalue_reference_t<Rng>>);
			CHECK_EQUAL(rng, {V{0, "hello", "john"},
			                   {1, "goodbye", "paul"},
			                   {2, "hello", "george"},
			                   {3, "goodbye", "ringo"}});
		}

		// Mixed ranges and common ranges
		{
			std::stringstream str{"john paul george ringo"};
			auto rng = view::zip(vi, vs, view::istream<std::string>(str));
			using Rng = decltype(rng);
			static_assert(View<Rng>);
			static_assert(InputRange<Rng>);
			static_assert(!ForwardRange<Rng>);
			static_assert(!SizedRange<Rng>);
			static_assert(!CommonRange<Rng>);
			using I = decltype(begin(rng));
			static_assert(InputIterator<I>);
			static_assert(!ForwardIterator<I>);
			using V = std::tuple<int, std::string, std::string>;
			static_assert(Same<V, value_type_t<I>>);
			std::vector<V> expected;
			ranges::copy(rng, ranges::back_inserter(expected));
			CHECK_EQUAL(expected, {V{0, "hello", "john"},
			                        {1, "goodbye", "paul"},
			                        {2, "hello", "george"},
			                        {3, "goodbye", "ringo"}});
		}

		{
			auto rnd_rng = view::zip(vi, vs);
			using Rng = decltype(rnd_rng);
			static_assert(RandomAccessRange<Rng>);
			static_assert(!ext::ContiguousRange<Rng>);
			static_assert(CommonRange<Rng>);
			static_assert(SizedRange<Rng>);
			using Ref = range_reference_t<decltype(rnd_rng)>;
			static_assert(Same<Ref, __tuple_hack<int &, std::string const &>>);
#if 0 // FIXME: bug in ranges::begin
			std::as_const(rnd_rng).begin();                 // Fine
			static_assert(__begin::has_member<const Rng&>); // Fine
			begin(std::as_const(rnd_rng));                  // Error??!?
			auto tmp = cbegin(rnd_rng) + 3;                 // Error
#else
			auto tmp = begin(rnd_rng) + 3;
#endif
			CHECK(std::get<0>(*tmp) == 3);
			CHECK(std::get<1>(*tmp) == "goodbye");

			CHECK((rnd_rng.end() - rnd_rng.begin()) == 4);
			CHECK((rnd_rng.begin() - rnd_rng.end()) == -4);
			CHECK(rnd_rng.size() == 4);
		}
#if 0 // TODO: zip_with
		// zip_with
		{
			std::vector<std::string> v0{"a", "b", "c"};
			std::vector<std::string> v1{"x", "y", "z"};

			auto rng = view::zip_with(std::plus<std::string>{}, v0, v1);
			std::vector<std::string> expected;
			copy(rng, ranges::back_inserter(expected));
			CHECK_EQUAL(expected, {"ax", "by", "cz"});
		}

		// zip_with
		{
			std::vector<std::string> v0{"a", "b", "c"};
			std::vector<std::string> v1{"x", "y", "z"};

			auto rng = view::zip_with(std::plus<std::string>{}, v0, v1);
			std::vector<std::string> expected;
			copy(rng, ranges::back_inserter(expected));
			CHECK_EQUAL(expected, {"ax", "by", "cz"});
		}
#endif // TODO

		// Move from a zip view
		{
			std::vector<MoveOnlyString> v0;
			for (auto p : {"a", "b", "c"}) v0.emplace_back(p);
			std::vector<MoveOnlyString> v1;
			for (auto p : {"x", "y", "z"}) v1.emplace_back(p);

			auto rng = view::zip(v0, v1);
			using R = decltype(rng);
			static_assert(RandomAccessRange<R>);
			static_assert(!ext::ContiguousRange<R>);
			std::vector<std::tuple<MoveOnlyString, MoveOnlyString>> expected;
			ranges::move(rng, ranges::back_inserter(expected));
			CHECK_EQUAL(expected, {std::tuple<std::string, std::string>{"a", "x"}, {"b", "y"}, {"c", "z"}});
			CHECK_EQUAL(v0, {"", "", ""});
			CHECK_EQUAL(v1, {"", "", ""});

			move(expected, rng.begin());
			CHECK_EQUAL(expected, {std::tuple<std::string, std::string>{"", ""}, {"", ""}, {"", ""}});
			CHECK_EQUAL(v0, {"a", "b", "c"});
			CHECK_EQUAL(v1, {"x", "y", "z"});

			std::vector<MoveOnlyString> res;
			auto proj =
				[](range_reference_t<R> p) -> MoveOnlyString& { return std::get<0>(p); };
			auto rng2 = rng | view::transform(proj);
			using R2 = decltype(rng2);
			static_assert(Same<range_value_type_t<R2>, MoveOnlyString>);
			static_assert(Same<range_reference_t<R2>, MoveOnlyString &>);
			static_assert(Same<range_rvalue_reference_t<R2>, MoveOnlyString &&>);
			move(rng2, ranges::back_inserter(res));
			CHECK_EQUAL(res, {"a", "b", "c"});
			CHECK_EQUAL(v0, {"", "", ""});
			CHECK_EQUAL(v1, {"x", "y", "z"});
		}
#if 0 // FIXME
		{
			auto const v = to_<std::vector<MoveOnlyString>>({"a", "b", "c"});
			auto rng = view::zip(v, v);
			using Rng = decltype(rng);
			using I = iterator_t<Rng>;
			static_assert(Readable<I>);
			static_assert(Same<
				range_value_type_t<Rng>,
				std::pair<MoveOnlyString, MoveOnlyString>>);
			static_assert(Same<
				range_reference_t<Rng>,
				common_pair<MoveOnlyString const &, MoveOnlyString const &>>);
			static_assert(Same<
				range_rvalue_reference_t<Rng>,
				common_pair<MoveOnlyString const &&, MoveOnlyString const &&>>);
			static_assert(Same<
				range_common_reference_t<Rng>,
				common_pair<MoveOnlyString const &, MoveOnlyString const &>>);
		}

		{
			std::vector<int> v{1, 2, 3, 4};
			auto moved = v | view::move;
			using Moved = decltype(moved);
			static_assert(Same<range_reference_t<Moved>, int &&>);
			auto zipped = view::zip(moved);
			using Zipped = decltype(zipped);
			static_assert(Same<range_reference_t<Zipped>, __tuple_hack<int &&> >);
		}

		// This is actually a test of the logic of view_adaptor. Since the stride view
		// does not redefine the current member function, the base range's iter_move
		// function gets picked up automatically.
		{
			auto rng0 = view::zip(vi, vs);
			auto rng1 = view::stride(rng0, 2);
			static_assert(Same<range_rvalue_reference_t<decltype(rng1)>, range_rvalue_reference_t<decltype(rng0)>>);
			static_assert(Same<range_value_type_t<decltype(rng1)>, range_value_type_t<decltype(rng0)>>);
		}

		// Test for noexcept iter_move
		{
			static_assert(noexcept(std::declval<std::unique_ptr<int>&>() = std::declval<std::unique_ptr<int>&&>()), "");
			std::unique_ptr<int> rg1[10], rg2[10];
			auto x = view::zip(rg1, rg2);
			std::pair<std::unique_ptr<int>, std::unique_ptr<int>> p = iter_move(x.begin());
			auto it = x.begin();
			static_assert(noexcept(iter_move(it)), "");
		}

		// Really a test for common_iterator's iter_move, but this is a good place for it.
		{
			std::unique_ptr<int> rg1[10], rg2[10];
			auto rg3 = rg2 | view::take_while([](std::unique_ptr<int> &){return true;});
			auto x = view::zip(rg1, rg3);
			::models_not<concepts::CommonRange>(x);
			auto y = x | view::common;
			std::pair<std::unique_ptr<int>, std::unique_ptr<int>> p = iter_move(y.begin());
			auto it = x.begin();
			static_assert(noexcept(iter_move(it)), "");
		}

		// Regression test for #439.
		{
			std::vector<int> vec{0, 1, 2};
			auto rng = vec | view::for_each([](int i) { return ranges::yield(i); });
			ranges::distance(view::zip(view::ints(0), rng) | view::common);
		}

		{
			int const i1[] = {0, 1, 2, 3};
			int const i2[] = {4, 5, 6, 7};
			auto rng = view::zip(
				debug_input_view<int const>{i1},
				debug_input_view<int const>{i2}
			);
			using P = std::pair<int, int>;
			CHECK_EQUAL(rng, {P{0, 4}, P{1, 5}, P{2, 6}, P{3, 7}});
		}
#endif
	}
}

int main() {
	using namespace ranges;

	{
		const int xs[] = {0, 1, 2, 3};
		int ys[] = {4, 5, 6, 7, 8, 9, 10};
		auto rng = view::zip(xs, ys);
		CHECK(size(rng) == 4);
		CHECK_EQUAL(rng, {std::tuple<int, int>{0, 4}, {1, 5}, {2, 6}, {3, 7}});
	}

	{
		int xs[] = {3, 2, 1, 0};
		int ys[] = {4, 5, 6, 7, 8, 9, 10};

		auto rng = view::zip(xs, ys);
		CHECK(size(rng) == 4);
		ranges::sort(rng, less{},
			[](auto&& x) { return std::get<0>(x); });

		CHECK_EQUAL(xs, {0, 1, 2, 3});
		CHECK_EQUAL(ys, {7, 6, 5, 4, 8, 9, 10});
	}

	{
		auto rng = view::zip();
		using R = decltype(rng);
		static_assert(Same<value_type_t<iterator_t<R>>, std::tuple<>>);
		CHECK(empty(rng));
		ranges::sort(rng);
		CHECK_EQUAL(rng, std::initializer_list<std::tuple<>>{});
	}

	range_v3_tests::test();

	return test_result();
}
