// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015 -- 2017
//  Copyright Christopher Di Bella 2016 -- 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/numeric/gcd.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"

namespace ranges = __stl2;

int main()
{
	// TODO: once constexpr lambdas are supported, change CHECK to static_assert
	CHECK(ranges::gcd(10, 12) == 2);
	CHECK(ranges::gcd(10, 13) == 1);
	CHECK(ranges::gcd(-20, -40) == 20);
	CHECK(ranges::gcd(-24, 24) == 24);
	CHECK(ranges::gcd(24, -8) ==  8);
	CHECK(ranges::gcd(17,  0) ==  0);
	CHECK(ranges::gcd( 0, 17) ==  0);

	CHECK(ranges::gcd(126, 2) == 2);
	CHECK(ranges::gcd(123, 456u) == std::common_type_t<int, unsigned int>{3});
	CHECK(ranges::gcd(125, -451) == 1);
	CHECK(ranges::gcd(-47, 468ll) == 1);
	CHECK(ranges::gcd(-8, -924) == 4);

	CHECK(ranges::gcd(68, -106) == 2);
	CHECK(ranges::gcd( 99, -680) == 1);
	CHECK(ranges::gcd(7, -698) == 1);
	CHECK(ranges::gcd(7, -522) == 1);
	CHECK(ranges::gcd(37, -985) == 1);

	CHECK(ranges::gcd(-41, 91) == 1);
	CHECK(ranges::gcd(-116, 794) == 2);
	CHECK(ranges::gcd(-118, 333) == 1);
	CHECK(ranges::gcd( -50, 38) == 2);
	CHECK(ranges::gcd( -32, 33) == 1);

	CHECK(ranges::gcd(-41, -91) == 1);
	CHECK(ranges::gcd(-116, -794) == 2);
	CHECK(ranges::gcd(-118, -333) == 1);
	CHECK(ranges::gcd( -50, -38) == 2);
	CHECK(ranges::gcd( -32, -33) == 1);

	CHECK(ranges::gcd(154, 86) == 2);
	CHECK(ranges::gcd(116, 515) == 1);
	CHECK(ranges::gcd(125, 362) == 1);
	CHECK(ranges::gcd(130, 50) == 10);
	CHECK(ranges::gcd(11, 710) == 1);

	CHECK(ranges::gcd(74, 81) == 1);
	CHECK(ranges::gcd(161, 275) == 1);
	CHECK(ranges::gcd(180, 941) == 1);
	CHECK(ranges::gcd(253, 595) ==  1);
	CHECK(ranges::gcd(243, 191) == 1);

	CHECK(ranges::gcd(289, 80) == 1);
	CHECK(ranges::gcd(772, 291) == 1);
	CHECK(ranges::gcd(243, 136) == 1);
	CHECK(ranges::gcd(243, 946) == 1);
	CHECK(ranges::gcd(794, 917) == 1);

	CHECK(ranges::gcd(612, 605) == 1);
	CHECK(ranges::gcd(724, 580) == 4);
	CHECK(ranges::gcd(554, 219) == 1);
	CHECK(ranges::gcd(440, 372) == 4);
	CHECK(ranges::gcd(351, 752) == 1);

	CHECK(ranges::gcd(-262, -728) == 2);
	CHECK(ranges::gcd(-990, -363) == 33);
	CHECK(ranges::gcd(-390, -680) == 10);
	CHECK(ranges::gcd(-281, -783) == 1);
	CHECK(ranges::gcd(-553, -785) == 1);

	CHECK(ranges::gcd(994, 51) == 1);
	CHECK(ranges::gcd(9684, 2432) == 4);
	CHECK(ranges::gcd(700, 614u) == std::common_type_t<int, unsigned int>{2});
	CHECK(ranges::gcd(451, 639ul) == std::common_type_t<int, unsigned long>{1});
	CHECK(ranges::gcd(155, 208ull) == std::common_type_t<int, unsigned long long>{1});

	CHECK(ranges::gcd(757, 67) == 1);
	CHECK(ranges::gcd(109, 40) == 1);
	CHECK(ranges::gcd(993, 220) == 1);
	CHECK(ranges::gcd(870ul, 665) == std::common_type_t<unsigned long, int>{5});
	CHECK(ranges::gcd(695, 886ll) == 1);

	CHECK(ranges::gcd(538, 567) == 1);
	CHECK(ranges::gcd(326, 112) == 2);
	CHECK(ranges::gcd(349, 485l) == 1);
	CHECK(ranges::gcd(178, 313) == 1);
	CHECK(ranges::gcd(574, 558) == 2);

	CHECK(ranges::gcd(852, 230) == 2);
	CHECK(ranges::gcd(646, 658) == 2);
	CHECK(ranges::gcd(847, 769) == 1);
	CHECK(ranges::gcd(201, 164) == 1);
	CHECK(ranges::gcd(967, 713) == 1);

	CHECK(ranges::gcd(124, 215) == 1);
	CHECK(ranges::gcd(207, 39) == 3);
	CHECK(ranges::gcd(852, 562) == 2);
	CHECK(ranges::gcd(422, 907) == 1);
	CHECK(ranges::gcd(947, 190) == 1);

	CHECK(ranges::gcd(621, 675) == 27);
	CHECK(ranges::gcd(580, 928) == 116);
	CHECK(ranges::gcd(765, 964) == 1);
	CHECK(ranges::gcd(21, 498) == 3);
	CHECK(ranges::gcd(1, 808) == 1);

	CHECK(ranges::gcd(462, 125) == 1);
	CHECK(ranges::gcd(835, 753) == 1);
	CHECK(ranges::gcd(723l, 356) == 1);
	CHECK(ranges::gcd(734l, 994l) == 2);
	CHECK(ranges::gcd(731l, 238ll) == 17);

	CHECK(ranges::gcd(531, 770) == 1);
	CHECK(ranges::gcd(833, 209) == 1);
	CHECK(ranges::gcd(857, 40) == 1);
	CHECK(ranges::gcd(467, 58) == 1);
	CHECK(ranges::gcd(227, 155) == 1);

	CHECK(ranges::gcd(820, 820) == 820);
	CHECK(ranges::gcd(776, 590) == 2);
	CHECK(ranges::gcd(109, 436) == 109);
	CHECK(ranges::gcd(249, 845) == 1);
	CHECK(ranges::gcd(221, 909) == 1);

	CHECK(ranges::gcd(586, 49) == 1);
	CHECK(ranges::gcd(877, 638) == 1);
	CHECK(ranges::gcd(579, 381) == 3);
	CHECK(ranges::gcd(423, 651) == 3);
	CHECK(ranges::gcd(136, 892) == 4);

	CHECK(ranges::gcd(657, 808) == 1);
	CHECK(ranges::gcd(120, 909) == 3);
	CHECK(ranges::gcd(797, 426) == 1);
	CHECK(ranges::gcd(491, 516) == 1);
	CHECK(ranges::gcd(53, 257) == 1);

	CHECK(ranges::gcd(790, 605) == 5);
	CHECK(ranges::gcd(493, 892) == 1);
	CHECK(ranges::gcd(827, 899) == 1);
	CHECK(ranges::gcd(29, 726) == 1);
	CHECK(ranges::gcd(600, 327) == 3);

	CHECK(ranges::gcd(299, 107) == 1);
	CHECK(ranges::gcd(214, 617) == 1);
	CHECK(ranges::gcd(308, 472) == 4);
	CHECK(ranges::gcd(652, 748l) == 4);
	CHECK(ranges::gcd(980, 601l) == 1);

	CHECK(ranges::gcd(839, 116) == 1);
	CHECK(ranges::gcd(349, 784) == 1);
	CHECK(ranges::gcd(771, 645) == 3);
	CHECK(ranges::gcd(908, 309) == 1);
	CHECK(ranges::gcd(793, 424) == 1);

	CHECK(ranges::gcd(410, 454) == 2);
	CHECK(ranges::gcd(628, 909) == 1);
	CHECK(ranges::gcd(505, 593) == 1);
	CHECK(ranges::gcd(908, 968) == 4);
	CHECK(ranges::gcd(558, 399) == 3);

	CHECK(ranges::gcd(850, 75) == 25);
	CHECK(ranges::gcd(318, 723) == 3);
	CHECK(ranges::gcd(984ll, 906u) == 6);
	CHECK(ranges::gcd(280ll, 511l) == 7);
	CHECK(ranges::gcd(975ll, 231ll) == 3);

	CHECK(ranges::gcd(160, 326) == 2);
	CHECK(ranges::gcd(833, 908) == 1);
	CHECK(ranges::gcd(242, 141) == 1);
	CHECK(ranges::gcd(868, 549) == 1);
	CHECK(ranges::gcd(59, 433) == 1);

	CHECK(ranges::gcd(501, 822) == 3);
	CHECK(ranges::gcd(808, 320) == 8);
	CHECK(ranges::gcd(778, 198) == 2);
	CHECK(ranges::gcd(389, 878) == 1);
	CHECK(ranges::gcd(89, 883) == 1);

	return ::test_result();
}
