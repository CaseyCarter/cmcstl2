#include <stl2/view/zip_with.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/move.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <stl2/view/common.hpp>
#include <stl2/view/istream.hpp>
#include <stl2/view/transform.hpp>

#include <stl2/detail/range/concepts.hpp>

#include "../simple_test.hpp"
#include "../test_utils.hpp"

#include <istream>
#include <sstream>
#include <string>
#include <vector>

namespace ranges = __stl2;

namespace view {
	using namespace ranges::view;
	using ranges::view::ext::zip;
	using ranges::view::ext::zip_with;
} // namespace view

template<ranges::ViewableRange> void models_ViewableRange();

int main()
{
	std::vector<int> vi{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::vector<std::string> const vs{"hello", "goodbye", "hello", "goodbye"};

	// All common ranges, but one single-pass
	// {
	// 	std::stringstream str{"john paul george ringo"};
	// 	using V = std::tuple<int, std::string, std::string>;
	// 	auto&& r = view::zip(vi, vs, view::istream<std::string>(str) | view::common);
	// TODO: find out why this breaks												^~~~~~~~~~~~~^

	// 	using R = decltype(r);
	// 	static_assert(ranges::CommonRange<R>);
	// 	static_assert(!ranges::SizedRange<R>);
	// 	static_assert(ranges::Same<ranges::iter_value_t<ranges::iterator_t<R>>, std::tuple<int&, std::string const&, std::string&>>);
	// 	//											I think this being non-reference-to-const means that I stuffed up somewhere   ^~~~~~~~~~~^

	// 	// static_assert(Same<ranges::iter_reference_t<R>, common_tuple<int &, std::string const &, std::string &>>());
	// 	// CONCEPT_ASSERT(Same<
	// 	// 	range_rvalue_reference_t<R>,
	// 	// 	common_tuple<int &&, std::string const &&, std::string &&>>());
	// 	static_assert(ranges::ConvertibleTo<ranges::iter_value_t<ranges::iterator_t<R>>&&, ranges::iter_value_t<ranges::iterator_t<R>>>); // Can this be Movable<R> instead?
	// 	static_assert(ranges::InputIterator<decltype(begin(r))>);
	// 	static_assert(!ranges::ForwardIterator<decltype(begin(r))>);

	// 	auto common = view::common(r);
	// 	auto expected = std::vector(ranges::begin(common), ranges::end(common));
	// 	CHECK_EQUAL(expected, {V{0, "hello", "john"},
	// 								  V{1, "goodbye", "paul"},
	// 								  V{2, "hello", "george"},
	// 								  V{3, "goodbye", "ringo"}});
	// }

	// consolation for the above not working
	{
		std::vector<std::string> vs2{"john", "paul", "george", "ringo"};
		auto&& r = view::zip(vi, vs, vs2);
		using R = decltype(r);
		static_assert(ranges::CommonRange<R>);
		static_assert(ranges::SizedRange<R>);
		// static_assert(ranges::Same<ranges::ext::range_value_t<R>, std::tuple<int, std::string, std::string>>);
		static_assert(ranges::Same<ranges::ext::range_reference_t<R>, ranges::common_tuple<int&, std::string const&, std::string&>>);

		static_assert(ranges::ConvertibleTo<ranges::iter_value_t<ranges::iterator_t<R>>&&, ranges::iter_value_t<ranges::iterator_t<R>>>); // Can this be Movable<R> instead?
		static_assert(ranges::RandomAccessIterator<decltype(ranges::begin(r))>);

		auto common = view::common(r);
		using V = std::tuple<int, std::string, std::string>;
		auto expected = std::vector<V>(ranges::begin(common), ranges::end(common));
		CHECK_EQUAL(expected, {V{0, "hello", "john"},
									  V{1, "goodbye", "paul"},
									  V{2, "hello", "george"},
									  V{3, "goodbye", "ringo"}});
	}

	// Mixed ranges and common ranges
	// {
	// 	std::stringstream str{"john paul george ringo"};
	// 	using V = std::tuple<int, std::string, std::string>;
	// 	auto&& r = view::zip(vi, vs, view::istream<std::string>(str));
	// 	using R = std::remove_reference_t<decltype(r)>;
	// 	static_assert(ranges::View<R>);
	// 	static_assert(!ranges::SizedRange<R>);
	// 	static_assert(!ranges::CommonRange<R>);
	// 	static_assert(ranges::InputRange<R>);
	// 	static_assert(!ranges::ForwardRange<R>);
	// 	std::vector<V> expected;
	// 	ranges::copy(r, ranges::back_inserter(expected));
	// 	CHECK_EQUAL(expected, {V{0, "hello", "john"},
	// 								  V{1, "goodbye", "paul"},
	// 								  V{2, "hello", "george"},
	// 								  V{3, "goodbye", "ringo"}});
	// }

	{
		auto rnd_r = view::zip(vi, vs);
		using R = decltype(rnd_r);
		using Ref = ranges::iter_reference_t<ranges::iterator_t<R>>;
		static_assert(std::is_same_v<Ref, ranges::common_tuple<int&, std::string const&>>);
		static_assert(ranges::CommonRange<R>);
		static_assert(ranges::SizedRange<R>);
		static_assert(ranges::RandomAccessIterator<ranges::iterator_t<R>>);
		auto tmp = ranges::cbegin(rnd_r) + 3;
		CHECK(std::get<0>(*tmp) == 3);
		CHECK(std::get<1>(*tmp) == "goodbye");

		CHECK((rnd_r.end() - rnd_r.begin()) == 4);
		CHECK((rnd_r.begin() - rnd_r.end()) == -4);
		CHECK(rnd_r.size() == 4u);
	}

	{
		std::vector<std::string> v0{"a","b","c"};
		std::vector<std::string> v1{"x","y","z"};

		auto r = view::zip_with(std::plus<>{}, v0, v1);
		std::vector<std::string> expected;
		ranges::copy(r, ranges::back_inserter(expected));
		CHECK_EQUAL(expected, {"ax","by","cz"});
	}

	{
		std::vector<std::string> v0{"a","b","c"};
		std::vector<std::string> v1{"x","y","z"};

		auto r = view::zip_with(std::plus<std::string>{}, v0, v1);
		std::vector<std::string> expected;
		copy(r, ranges::back_inserter(expected));
		CHECK_EQUAL(expected, {"ax","by","cz"});
	}

	// Move from a zip view
	{
		static_assert(ranges::Movable<move_only_string>);
		auto v0 = std::vector<std::string>{};
		// models_ViewableRange<
		// 	ranges::all_view<
		// 		std::vector<move_only_string>
		// 	>
		// >();
		v0.emplace_back("a");
		v0.emplace_back("b");
		v0.emplace_back("c");

		auto v1 = std::vector<std::string>{};
		v1.emplace_back("x");
		v1.emplace_back("y");
		v1.emplace_back("z");

		auto r = view::zip(v0, v1);

		using R = decltype(r);
		static_assert(ranges::RandomAccessRange<R>);

		{
			// ATTN @CaseyCarter
			std::vector<std::tuple<std::string, std::string>> expected;
			ranges::move(r, ranges::back_inserter(expected));
			CHECK_EQUAL(expected | view::keys, {"a","b","c"});
			CHECK_EQUAL(expected | view::values, {"x","y","z"});
			CHECK_EQUAL(v0, {"","",""});
			CHECK_EQUAL(v1, {"","",""});
		}

		// move(expected, r.begin());
		// CHECK_EQUAL(expected | view::keys, {"","",""});
		// CHECK_EQUAL(expected | view::values, {"","",""});
		// CHECK_EQUAL(v0, {"a","b","c"});
		// CHECK_EQUAL(v1, {"x","y","z"});

	// 	std::vector<move_only_string> res;
	// 	using R = decltype(r);
	// 	auto proj =
	// 		[](range_reference_t<R> p) -> move_only_string& {return p.first;};
	// 	auto r2 = r | view::transform(proj);
	// 	move(r2, ranges::back_inserter(res));
	// 	CHECK_EQUAL(res, {"a","b","c"});
	// 	CHECK_EQUAL(v0, {"","",""});
	// 	CHECK_EQUAL(v1, {"x","y","z"});
	// 	using R2 = decltype(r2);
	// 	CONCEPT_ASSERT(Same<range_value_type_t<R2>, move_only_string>());
	// 	CONCEPT_ASSERT(Same<range_reference_t<R2>, move_only_string &>());
	// 	CONCEPT_ASSERT(Same<range_rvalue_reference_t<R2>, move_only_string &&>());
	}

	// {
	// 	auto const v = to_<std::vector<move_only_string>>({"a","b","c"});
	// 	auto r = view::zip(v, v);
	// 	using R = decltype(r);
	// 	using I = iterator_t<R>;
	// 	CONCEPT_ASSERT(Readable<I>());
	// 	CONCEPT_ASSERT(Same<
	// 		iter_value_t<I>,
	// 		std::pair<move_only_string, move_only_string>>());
	// 	CONCEPT_ASSERT(Same<
	// 		iter_reference_t<I>,
	// 		common_pair<move_only_string const &, move_only_string const &>>());
	// 	CONCEPT_ASSERT(Same<
	// 		iter_rvalue_reference_t<I>,
	// 		common_pair<move_only_string const &&, move_only_string const &&>>());
	// 	CONCEPT_ASSERT(Same<
	// 		range_common_reference_t<R>,
	// 		common_pair<move_only_string const &, move_only_string const &>>());
	// }

	// {
	// 	std::vector<int> v{1,2,3,4};
	// 	auto moved = v | view::move;
	// 	using Moved = decltype(moved);
	// 	CONCEPT_ASSERT(Same<range_reference_t<Moved>, int &&>());
	// 	auto zipped = view::zip(moved);
	// 	using Zipped = decltype(zipped);
	// 	CONCEPT_ASSERT(Same<range_reference_t<Zipped>, common_tuple<int &&> >());
	// }

	// // This is actually a test of the logic of view_adaptor. Since the stride view
	// // does not redefine the current member function, the base range's iter_move
	// // function gets picked up automatically.
	// {
	// 	auto r0 = view::zip(vi, vs);
	// 	auto r1 = view::stride(r0, 2);
	// 	CONCEPT_ASSERT(Same<range_rvalue_reference_t<decltype(r1)>, range_rvalue_reference_t<decltype(r0)>>());
	// 	CONCEPT_ASSERT(Same<range_value_type_t<decltype(r1)>, range_value_type_t<decltype(r0)>>());
	// }

	// // Test for noexcept iter_move
	// {
	// 	static_assert(noexcept(std::declval<std::unique_ptr<int>&>() = std::declval<std::unique_ptr<int>&&>()), "");
	// 	std::unique_ptr<int> rg1[10], rg2[10];
	// 	auto x = view::zip(rg1, rg2);
	// 	std::pair<std::unique_ptr<int>, std::unique_ptr<int>> p = iter_move(x.begin());
	// 	auto it = x.begin();
	// 	static_assert(noexcept(iter_move(it)), "");
	// }

	// // Really a test for common_iterator's iter_move, but this is a good place for it.
	// {
	// 	std::unique_ptr<int> rg1[10], rg2[10];
	// 	auto rg3 = rg2 | view::take_while([](std::unique_ptr<int> &){return true;});
	// 	auto x = view::zip(rg1, rg3);
	// 	::models_not<concepts::BoundedRange>(x);
	// 	auto y = x | view::bounded;
	// 	std::pair<std::unique_ptr<int>, std::unique_ptr<int>> p = iter_move(y.begin());
	// 	auto it = x.begin();
	// 	static_assert(noexcept(iter_move(it)), "");
	// }

	// // Regression test for #439.
	// {
	// 	std::vector<int> vec{0,1,2};
	// 	auto r = vec | view::for_each([](int i) { return ranges::yield(i); });
	// 	ranges::distance(view::zip(view::ints(0), r) | view::bounded);
	// }

	// {
	// 	int const i1[] = {0,1,2,3};
	// 	int const i2[] = {4,5,6,7};
	// 	auto r = view::zip(
	// 		debug_input_view<int const>{i1},
	// 		debug_input_view<int const>{i2}
	// 	);
	// 	using P = std::pair<int, int>;
	// 	CHECK_EQUAL(r, {P{0,4},P{1,5}, P{2,6}, P{3,7}});
	// }

	return ::test_result();
}
