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
#include <stl2/view/join.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/transform.hpp>
#include <stl2/view/filter.hpp>
#include <stl2/detail/algorithm/count.hpp>
#include <stl2/detail/algorithm/transform.hpp>
#include <stl2/detail/iterator/insert_iterators.hpp>
#include <memory>
#include <vector>
#include <string>
#include "../simple_test.hpp"

namespace ranges = __stl2;

int main()
{
	using namespace ranges;

	{
		std::vector<std::string> vs{"this","is","his","face"};
		join_view jv{vs};
		CHECK_EQUAL(jv, {'t','h','i','s','i','s','h','i','s','f','a','c','e'});
		static_assert(bidirectional_range<decltype(jv)>);
		static_assert(bidirectional_range<const decltype(jv)>);
		static_assert(common_range<decltype(jv)>);
		static_assert(common_range<const decltype(jv)>);
	}

	{
		auto rng = views::iota(0,4)
			| views::transform([](int i) {return views::iota(0,i);})
			| views::join;
		CHECK_EQUAL(rng, {0,0,1,0,1,2});
		static_assert(input_range<decltype(rng)>);
		static_assert(!range<const decltype(rng)>);
		static_assert(!forward_range<decltype(rng)>);
		static_assert(!common_range<decltype(rng)>);
	}

	{
		auto rng = views::iota(0,4)
			| views::transform([](int i) {return views::iota(0,i);})
			| views::filter([](auto){ return true; })
			| views::join;
		CHECK_EQUAL(rng, {0,0,1,0,1,2});
		static_assert(input_range<decltype(rng)>);
		static_assert(!range<const decltype(rng)>);
		static_assert(!forward_range<decltype(rng)>);
		static_assert(!common_range<decltype(rng)>);
	}

	{
		// https://github.com/ericniebler/stl2/issues/604
		auto rng0 = views::iota(0, 4)
			| views::transform([](int i) { return views::iota(0, i); });
		auto rng1 = ref_view{rng0};
		static_assert(random_access_range<decltype(rng1)>);
		static_assert(range<const decltype(rng1)>);
		static_assert(common_range<decltype(rng1)>);
		static_assert(random_access_range<ext::range_reference_t<decltype(rng1)>>);
		static_assert(ext::SimpleView<decltype(rng1)>);
		static_assert(!std::is_reference_v<ext::range_reference_t<decltype(rng1)>>);
		auto rng2 = rng1 | views::join;
		CHECK_EQUAL(rng2, {0,0,1,0,1,2});
		static_assert(input_range<decltype(rng2)>);
		static_assert(!range<const decltype(rng2)>);
		static_assert(!forward_range<decltype(rng2)>);
		static_assert(!common_range<decltype(rng2)>);
	}

	return ::test_result();
}
