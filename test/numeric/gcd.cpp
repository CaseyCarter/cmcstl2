#include <stl2/detail/numeric/gcd.hpp>
#include "../simple_test.hpp"

int main()
{
   // TODO: once constexpr lambdas are supported, change to static_assert
   // TODO: change some of the types to non-int
   CHECK(__stl2::gcd(10, 12) == 2);
   CHECK(__stl2::gcd(10, 13) == 1);
   CHECK(__stl2::gcd(20, 40) == 20);
   CHECK(__stl2::gcd(24, 24) == 24);
   CHECK(__stl2::gcd(24,  8) ==  8);
   CHECK(__stl2::gcd(17,  0) ==  0);
   CHECK(__stl2::gcd( 0, 17) ==  0);
   CHECK(__stl2::gcd(123, 456) == 3);
   CHECK(__stl2::gcd(9684, 2434) == 2);
   CHECK(__stl2::gcd(853, 595) == 1);
   CHECK(__stl2::gcd(50, 38) == 2);
   CHECK(__stl2::gcd(870, 665) == 5);
   CHECK(__stl2::gcd(11, 710) == 1);
   CHECK(__stl2::gcd(289, 80) == 1);
   CHECK(__stl2::gcd(243, 136) == 1);
   CHECK(__stl2::gcd(243, 946) == 1);
   CHECK(__stl2::gcd(794, 917) == 1);
   CHECK(__stl2::gcd(125, 451) == 1);
   CHECK(__stl2::gcd(994, 51) == 1);
   CHECK(__stl2::gcd(700, 614) == 2);
   CHECK(__stl2::gcd(451, 639) == 1);
   CHECK(__stl2::gcd(155, 208) == 1);
   CHECK(__stl2::gcd(757, 67) == 1);
   CHECK(__stl2::gcd(109, 40) == 1);
   CHECK(__stl2::gcd(8, 924) == 4);
   CHECK(__stl2::gcd(130, 500) == 10);
   CHECK(__stl2::gcd(695, 886) == 1);
   CHECK(__stl2::gcd(723, 356) == 1);
   CHECK(__stl2::gcd(207, 39) == 3);
   CHECK(__stl2::gcd(852, 562) == 2);
   CHECK(__stl2::gcd(422, 907) == 1);
   CHECK(__stl2::gcd(947, 190) == 1);
   CHECK(__stl2::gcd(462, 125) == 1);
   CHECK(__stl2::gcd(835, 753) == 1);
   CHECK(__stl2::gcd(124, 215) == 1);
   CHECK(__stl2::gcd(734, 994) == 2);
   CHECK(__stl2::gcd(731, 238) == 17);
   CHECK(__stl2::gcd(116, 515) == 1);
   CHECK(__stl2::gcd(877, 638) == 1);
   CHECK(__stl2::gcd(579, 381) == 3);
   CHECK(__stl2::gcd(423, 651) == 3);
   CHECK(__stl2::gcd(136, 892) == 4);
   CHECK(__stl2::gcd(308, 472) == 4);
   CHECK(__stl2::gcd(214, 617) == 1);
   CHECK(__stl2::gcd(299, 107) == 1);
   CHECK(__stl2::gcd(652, 748) == 4);
   CHECK(__stl2::gcd(980, 601) == 1);
   CHECK(__stl2::gcd(797, 426) == 1);
   CHECK(__stl2::gcd(318, 723) == 3);
   CHECK(__stl2::gcd(984, 906) == 6);
   CHECK(__stl2::gcd(280, 511) == 7);
   CHECK(__stl2::gcd(975, 231) == 3);
   CHECK(__stl2::gcd(612, 605) == 1);
   CHECK(__stl2::gcd(724, 580) == 4);
   CHECK(__stl2::gcd(554, 219) == 1);
   CHECK(__stl2::gcd(440, 372) == 4);
   CHECK(__stl2::gcd(351, 752) == 1);
   CHECK(__stl2::gcd(262, 728) == 2);
   CHECK(__stl2::gcd(798, 691) == 1);
   CHECK(__stl2::gcd(7, 522) == 1);
   CHECK(__stl2::gcd(281, 783) == 1);
   CHECK(__stl2::gcd(553, 785) == 1);
   CHECK(__stl2::gcd(409, 665) == 1);
   CHECK(__stl2::gcd(861, 275) == 1);
   CHECK(__stl2::gcd(580, 941) == 1);
   CHECK(__stl2::gcd(586, 49) == 1);
   CHECK(__stl2::gcd(443, 191) == 1);
   CHECK(__stl2::gcd(538, 567) == 1);
   CHECK(__stl2::gcd(326, 112) == 2);
   CHECK(__stl2::gcd(126, 2) == 2);
   CHECK(__stl2::gcd(178, 313) == 1);
   CHECK(__stl2::gcd(574, 558) == 2);
   CHECK(__stl2::gcd(852, 230) == 2);
   CHECK(__stl2::gcd(47, 468) == 1);
   CHECK(__stl2::gcd(847, 769) == 1);
   CHECK(__stl2::gcd(201, 164) == 1);
   CHECK(__stl2::gcd(967, 713) == 1);
   CHECK(__stl2::gcd(205, 273) == 1);
   CHECK(__stl2::gcd(526, 462) == 2);
   CHECK(__stl2::gcd(225, 899) == 1);
   CHECK(__stl2::gcd(342, 673) == 1);
   CHECK(__stl2::gcd(416, 114) == 2);
   CHECK(__stl2::gcd(7, 698) == 1);
   CHECK(__stl2::gcd(580, 928) == 116);
   CHECK(__stl2::gcd(765, 964) == 1);
   CHECK(__stl2::gcd(21, 498) == 3);
   CHECK(__stl2::gcd(1, 808) == 1);
   CHECK(__stl2::gcd(531, 770) == 1);
   CHECK(__stl2::gcd(833, 209) == 1);
   CHECK(__stl2::gcd(857, 40) == 1);
   CHECK(__stl2::gcd(467, 58) == 1);
   CHECK(__stl2::gcd(227, 155) == 1);
   CHECK(__stl2::gcd(125, 362) == 1);
   CHECK(__stl2::gcd(776, 590) == 2);
   CHECK(__stl2::gcd(109, 436) == 109);
   CHECK(__stl2::gcd(249, 845) == 1);
   CHECK(__stl2::gcd(221, 909) == 1);
   CHECK(__stl2::gcd(657, 808) == 1);
   CHECK(__stl2::gcd(120, 909) == 3);
   CHECK(__stl2::gcd(850, 75) == 25);
   CHECK(__stl2::gcd(491, 516) == 1);
   CHECK(__stl2::gcd(53, 257) == 1);
   CHECK(__stl2::gcd(790, 605) == 5);
   CHECK(__stl2::gcd(493, 892) == 1);
   CHECK(__stl2::gcd(827, 899) == 1);
   CHECK(__stl2::gcd(29, 726) == 1);
   CHECK(__stl2::gcd(600, 327) == 3);
   CHECK(__stl2::gcd(839, 116) == 1);
   CHECK(__stl2::gcd(349, 784) == 1);
   CHECK(__stl2::gcd(771, 645) == 3);
   CHECK(__stl2::gcd(908, 309) == 1);
   CHECK(__stl2::gcd(793, 424) == 1);
   CHECK(__stl2::gcd(410, 454) == 2);
   CHECK(__stl2::gcd(628, 909) == 1);
   CHECK(__stl2::gcd(505, 593) == 1);
   CHECK(__stl2::gcd(908, 968) == 4);
   CHECK(__stl2::gcd(558, 399) == 3);
   CHECK(__stl2::gcd(160, 326) == 2);
   CHECK(__stl2::gcd(833, 908) == 1);
   CHECK(__stl2::gcd(242, 141) == 1);
   CHECK(__stl2::gcd(868, 549) == 1);
   CHECK(__stl2::gcd(59, 433) == 1);
   CHECK(__stl2::gcd(501, 822) == 3);
   CHECK(__stl2::gcd(808, 320) == 8);
   CHECK(__stl2::gcd(778, 198) == 2);
   CHECK(__stl2::gcd(389, 878) == 1);
   CHECK(__stl2::gcd(89, 883) == 1);
   CHECK(__stl2::gcd(390, 680) == 10);
   CHECK(__stl2::gcd(154, 86) == 2);
   CHECK(__stl2::gcd(646, 658) == 2);
   CHECK(__stl2::gcd(772, 291) == 1);
   CHECK(__stl2::gcd(993, 220) == 1);
   CHECK(__stl2::gcd(136, 212) == 4);
   CHECK(__stl2::gcd(909, 680) == 1);
   CHECK(__stl2::gcd(621, 675) == 27);
   CHECK(__stl2::gcd(349, 485) == 1);
   CHECK(__stl2::gcd(158, 333) == 1);
   CHECK(__stl2::gcd(794, 116) == 2);
   CHECK(__stl2::gcd(990, 363) == 33);
   return ::test_result();
}
