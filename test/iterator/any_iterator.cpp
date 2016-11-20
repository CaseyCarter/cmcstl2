#include <stl2/detail/iterator/any_iterator.hpp>
#include <iterator>
#include <iostream>
#include <string>
#include <sstream>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

void test_small() {
    int rg[]{0,1,2,3,4,5,6,7,8,9};
    stl2::any_input_iterator<int&> first{rg}, last{rg+10};
    auto a1 = first, a2 = a1; (void)a2; // makin' copies
    CHECK(&rg[5] == &*stl2::next(first, 5));
    CHECK(&rg[5] == &(int const&)stl2::iter_move(stl2::next(first, 5)));
    int i = 0;
    for (; first != last && !(first == last); ++first, ++i)
        CHECK(*first == rg[i]);
    CHECK(i == 10);
    first = rg;
    CHECK(*first == 0);
}

void test_big() {
    std::stringstream sin{"now is the time for all good personages"};
    std::istream_iterator<std::string> first_{sin}, last_{};
    stl2::any_input_iterator<std::string const &> first{first_}, last{last_};
    auto a1 = first, a2 = a1; (void)a2; // makin' aliases
    CHECK((first != last && *first == "now"));
    first = stl2::next(first, 7);
    CHECK((first != last && *first == "personages"));
    CHECK(++first == last);
}

int main() {
	test_small();
	test_big();
	return ::test_result();
}
