#include <stl2/view/elements.hpp>
#include <vector>
#include <map>
#include <utility>
#include <array>

#include "../simple_test.hpp"

namespace ranges = __stl2;

int main()
{
    using namespace ranges;
    //using namespace ranges::ext;

    std::vector<std::tuple<int, int, int>> vector_of_tuple = {
        {0, 1, 2 },
        {3, 4, 5 },
        {6, 7, 8 },
        {9, 10, 11 }
    };

    std::vector<std::array<int, 3>> vector_of_array = {
        {0, 1, 2 },
        {3, 4, 5 },
        {6, 7, 8 },
        {9, 10, 11 }
    };

    std::map<std::string, int> map {
        {"A", 1},
        {"B", 2},
        {"C", 3},
        {"D", 4},
    };

    {
        auto rng0 = ranges::ext::view::elements<0>(vector_of_tuple);
        static_assert(Same<int&, decltype(*begin(rng0))>);
        static_assert(SizedRange<decltype(rng0)>);
        static_assert(RandomAccessRange<decltype(rng0)>);
        CHECK_EQUAL(rng0, {0, 3, 6, 9});

        auto rng1 = ranges::ext::view::elements<1>(vector_of_tuple);
        static_assert(Same<int&, decltype(*begin(rng1))>);
        static_assert(SizedRange<decltype(rng1)>);
        static_assert(RandomAccessRange<decltype(rng1)>);
        CHECK_EQUAL(rng1, {1, 4, 7, 10});

        auto rng2 = ranges::ext::view::elements<2>(vector_of_tuple);
        static_assert(Same<int&, decltype(*begin(rng2))>);
        static_assert(SizedRange<decltype(rng2)>);
        static_assert(RandomAccessRange<decltype(rng2)>);
        CHECK_EQUAL(rng2, {2, 5, 8, 11});
    }

    {
        auto rng0 = ranges::ext::view::elements<0>(vector_of_array);
        static_assert(Same<int&, decltype(*begin(rng0))>);
        static_assert(SizedRange<decltype(rng0)>);
        static_assert(RandomAccessRange<decltype(rng0)>);
        CHECK_EQUAL(rng0, {0, 3, 6, 9});
    }
    {
        auto keys = map | ranges::ext::view::keys;
        static_assert(Same<const std::string&, decltype(*begin(keys))>);
        static_assert(SizedRange<decltype(keys)>);
        static_assert(!RandomAccessRange<decltype(keys)>);
        CHECK_EQUAL(keys, {"A", "B", "C", "D"});

        auto values = map | ranges::ext::view::values;
        static_assert(Same<int&, decltype(*begin(values))>);
        static_assert(SizedRange<decltype(values)>);
        static_assert(!RandomAccessRange<decltype(values)>);
        CHECK_EQUAL(values, {1, 2, 3, 4});
    }
    return ::test_result();
}
