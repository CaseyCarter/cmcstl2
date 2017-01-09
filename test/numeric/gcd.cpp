#include <stl2/detail/numeric/gcd.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"

namespace ranges = __stl2;

template <typename M, typename N = M>
requires
   ranges::Number<M, N>()
void CHECK_gcd(M m, N n, std::common_type_t<M, N> expected) noexcept
{
   CHECK(ranges::gcd(m, n) == expected);
}

int main()
{
   // TODO: once constexpr lambdas are supported, change CHECK to static_assert
   CHECK(ranges::gcd(10, 12) == 2);
   CHECK(ranges::gcd(10, 13) == 1);
   CHECK(ranges::gcd(20, 40) == 20);
   CHECK(ranges::gcd(24, 24) == 24);
   CHECK(ranges::gcd(24,  8) ==  8);
   CHECK(ranges::gcd(17,  0) ==  0);
   CHECK(ranges::gcd( 0, 17) ==  0);

   CHECK_gcd(125u, 451u, 1);
   CHECK_gcd(853l, 595l, 1);
   CHECK_gcd(50ul, 38ul, 2);
   CHECK_gcd(870ll, 665ll, 5);
   CHECK_gcd(11ull, 710ull, 1);

   CHECK_gcd<short, signed char>(289, 80, 1);
   CHECK_gcd<short, short>(123, 456, 3);
   CHECK_gcd<short>(243, 136, 1);
   CHECK_gcd<short>(243, 946l, 1);
   CHECK_gcd<short>(794, 917ll, 1);

   CHECK_gcd<unsigned short, unsigned char>(994, 51, 1);
   CHECK_gcd<unsigned short, unsigned short>(9684, 2432, 4);
   CHECK_gcd<unsigned short>(700, 614u, 2);
   CHECK_gcd<unsigned short>(451, 639ul, 1);
   CHECK_gcd<unsigned short>(155, 208ull, 1);

   CHECK_gcd<int, signed char>(757, 67, 1);
   CHECK_gcd<int, short>(109, 40, 1);
   CHECK_gcd(8, 924, 4);
   CHECK_gcd(130, 500l, 10);
   CHECK_gcd(695, 886ll, 1);

   CHECK_gcd<unsigned int, unsigned char>(124, 215, 1);
   CHECK_gcd<unsigned int, unsigned short>(207, 39, 3);
   CHECK_gcd(852u, 562u, 2);
   CHECK_gcd(422u, 907ul, 1);
   CHECK_gcd(947u, 190ull, 1);

   CHECK_gcd<long, signed char>(462, 125, 1);
   CHECK_gcd<long, short>(835, 753, 1);
   CHECK_gcd(723l, 356, 1);
   CHECK_gcd(734l, 994l, 2);
   CHECK_gcd(731l, 238ll, 17);

   CHECK_gcd<unsigned long, unsigned char>(586, 49, 1);
   CHECK_gcd<unsigned long, unsigned short>(877, 638, 1);
   CHECK_gcd(579ul, 381u, 3);
   CHECK_gcd(423ul, 651ul, 3);
   CHECK_gcd(136ul, 892ull, 4);

   CHECK_gcd<long long, signed char>(299, 107, 1);
   CHECK_gcd<long long, short>(214, 617, 1);
   CHECK_gcd(308ll, 472, 4);
   CHECK_gcd(652ll, 748l, 4);
   CHECK_gcd(980ll, 601l, 1);

   CHECK_gcd<unsigned long long, unsigned char>(850, 75, 25);
   CHECK_gcd<unsigned long long, unsigned short>(318, 723, 3);
   CHECK_gcd(984ull, 906u, 6);
   CHECK_gcd(280ull, 511ul, 7);
   CHECK_gcd(975ull, 231ull, 3);

   CHECK_gcd(612, 605ull, 1);
   CHECK_gcd(724, 580, 4);
   CHECK_gcd(554, 219, 1);
   CHECK_gcd(440, 372, 4);
   CHECK_gcd(351, 752, 1);
   CHECK_gcd(262, 728, 2);
   CHECK_gcd(798, 691, 1);
   CHECK_gcd(7, 522, 1);
   CHECK_gcd(281, 783, 1);
   CHECK_gcd(553, 785, 1);
   CHECK_gcd(409, 665, 1);
   CHECK_gcd(861, 275, 1);
   CHECK_gcd(580, 941, 1);
   CHECK_gcd(116, 515, 1);
   CHECK_gcd(443, 191, 1);
   CHECK_gcd(538, 567, 1);
   CHECK_gcd(326, 112, 2);
   CHECK_gcd(126, 2, 2);
   CHECK_gcd(178, 313, 1);
   CHECK_gcd(574, 558, 2);
   CHECK_gcd(852, 230, 2);
   CHECK_gcd(47, 468, 1);
   CHECK_gcd(847, 769, 1);
   CHECK_gcd(201, 164, 1);
   CHECK_gcd(967, 713, 1);
   CHECK_gcd(205, 273, 1);
   CHECK_gcd(526, 462, 2);
   CHECK_gcd(225, 899, 1);
   CHECK_gcd(342, 673, 1);
   CHECK_gcd(416, 114, 2);
   CHECK_gcd(7, 698, 1);
   CHECK_gcd(580, 928, 116);
   CHECK_gcd(765, 964, 1);
   CHECK_gcd(21, 498, 3);
   CHECK_gcd(1, 808, 1);
   CHECK_gcd(531, 770, 1);
   CHECK_gcd(833, 209, 1);
   CHECK_gcd(857, 40, 1);
   CHECK_gcd(467, 58, 1);
   CHECK_gcd(227, 155, 1);
   CHECK_gcd(125, 362, 1);
   CHECK_gcd(776, 590, 2);
   CHECK_gcd(109, 436, 109);
   CHECK_gcd(249, 845, 1);
   CHECK_gcd(221, 909, 1);
   CHECK_gcd(657, 808, 1);
   CHECK_gcd(120, 909, 3);
   CHECK_gcd(797, 426, 1);
   CHECK_gcd(491, 516, 1);
   CHECK_gcd(53, 257, 1);
   CHECK_gcd(790, 605, 5);
   CHECK_gcd(493, 892, 1);
   CHECK_gcd(827, 899, 1);
   CHECK_gcd(29, 726, 1);
   CHECK_gcd(600, 327, 3);
   CHECK_gcd(839, 116, 1);
   CHECK_gcd(349, 784, 1);
   CHECK_gcd(771, 645, 3);
   CHECK_gcd(908, 309, 1);
   CHECK_gcd(793, 424, 1);
   CHECK_gcd(410, 454, 2);
   CHECK_gcd(628, 909, 1);
   CHECK_gcd(505, 593, 1);
   CHECK_gcd(908, 968, 4);
   CHECK_gcd(558, 399, 3);
   CHECK_gcd(160, 326, 2);
   CHECK_gcd(833, 908, 1);
   CHECK_gcd(242, 141, 1);
   CHECK_gcd(868, 549, 1);
   CHECK_gcd(59, 433, 1);
   CHECK_gcd(501, 822, 3);
   CHECK_gcd(808, 320, 8);
   CHECK_gcd(778, 198, 2);
   CHECK_gcd(389, 878, 1);
   CHECK_gcd(89, 883, 1);
   CHECK_gcd(390, 680, 10);
   CHECK_gcd(154, 86, 2);
   CHECK_gcd(646, 658, 2);
   CHECK_gcd(772, 291, 1);
   CHECK_gcd(993, 220, 1);
   CHECK_gcd(136, 212, 4);
   CHECK_gcd(909, 680, 1);
   CHECK_gcd(621, 675, 27);
   CHECK_gcd(349, 485, 1);
   CHECK_gcd(158, 333, 1);
   CHECK_gcd(794, 116, 2);
   CHECK_gcd(990, 363, 33);
   return ::test_result();
}
