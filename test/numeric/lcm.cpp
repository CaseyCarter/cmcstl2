// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015 -- 2017
//  Copyright Christopher Di Bella 2016 -- 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt);
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/numeric/lcm.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"

namespace ranges = __stl2;

int main()
{
   // TODO: once constexpr lambdas are supported, change CHECK to static_assert
   CHECK(ranges::lcm(0, 0) == 0);
   CHECK(ranges::lcm(0, 1) == 1);
   CHECK(ranges::lcm(1, 0) == 1);
   CHECK(ranges::lcm(79436, 6230) == 35349020);
   CHECK(ranges::lcm(64780, 3209) == 207879020);
   CHECK(ranges::lcm(45445, 8771) == 398598095);
   CHECK(ranges::lcm(44461, 2245) == 99814945);
   CHECK(ranges::lcm(17898, 1750) == 15660750);
   CHECK(ranges::lcm(1900, 660) == 62700);
   CHECK(ranges::lcm(28765, 6202) == 178400530);

   CHECK(ranges::lcm(23000, 9897) == 227631000);
   CHECK(ranges::lcm(73625, 4128) == 303924000);
   CHECK(ranges::lcm(90198, 765) == 7666830);
   CHECK(ranges::lcm(58507, 7301) == 427159607);
   CHECK(ranges::lcm(56898, 5597) == 10981314);

   CHECK(ranges::lcm(30173, 4906) == 13457158);
   CHECK(ranges::lcm(31611, 957) == 10083909);
   CHECK(ranges::lcm(78278, 6897) == 539883366);
   CHECK(ranges::lcm(17872, 3061) == 54706192);
   CHECK(ranges::lcm(86572, 9216) == 199461888);

   CHECK(ranges::lcm(81841, 7140) == 584344740);
   CHECK(ranges::lcm(43959, 7394) == 325032846);
   CHECK(ranges::lcm(56834, 5211) == 296161974);
   CHECK(ranges::lcm(36045, 8096) == 291820320);
   CHECK(ranges::lcm(14830, 1837) == 27242710);

   CHECK(ranges::lcm(58127, 3871) == 225009617);
   CHECK(ranges::lcm(16556, 9555) == 158192580);
   CHECK(ranges::lcm(74850, 8595) == 42889050);
   CHECK(ranges::lcm(95236, 5976) == 142282584);
   CHECK(ranges::lcm(89280, 608) == 1696320);

   CHECK(ranges::lcm(44691, 6932) == 309798012);
   CHECK(ranges::lcm(73990, 9435) == 139619130);
   CHECK(ranges::lcm(95433, 221) == 1622361);
   CHECK(ranges::lcm(42694, 9277) == 396072238);
   CHECK(ranges::lcm(40101, 2539) == 101816439);

   CHECK(ranges::lcm(84597, 1915) == 162003255);
   CHECK(ranges::lcm(14759, 8584) == 126691256);
   CHECK(ranges::lcm(92911, 5419) == 503484709);
   CHECK(ranges::lcm(83019, 1800) == 49811400);
   CHECK(ranges::lcm(30678, 2768) == 42458352);

   CHECK(ranges::lcm(9997, 1625) == 1249625);
   CHECK(ranges::lcm(60106, 5730) == 172203690);
   CHECK(ranges::lcm(91681, 1910) == 175110710);
   CHECK(ranges::lcm(59592, 634) == 18890664);
   CHECK(ranges::lcm(24999, 8603) == 215066397);

   CHECK(ranges::lcm(99669, 9875) == 984231375);
   CHECK(ranges::lcm(86718, 1503) == 43445718);
   CHECK(ranges::lcm(94419, 8065) == 761489235);
   CHECK(ranges::lcm(32115, 8122) == 260838030);
   CHECK(ranges::lcm(94867, 5629) == 534006343);

   CHECK(ranges::lcm(94661, 8804) == 833395444);
   CHECK(ranges::lcm(62735, 9327) == 585129345);
   CHECK(ranges::lcm(44083, 2737) == 120655171);
   CHECK(ranges::lcm(24925, 4948) == 123328900);
   CHECK(ranges::lcm(27485, 7634) == 209820490);

   CHECK(ranges::lcm(24800, 8479) == 210279200);
   CHECK(ranges::lcm(65098, 8300) == 270156700);
   CHECK(ranges::lcm(49725, 7573) == 376567425);
   CHECK(ranges::lcm(3151, 7887) == 24851937);
   CHECK(ranges::lcm(74695, 7994) == 597111830);

   CHECK(ranges::lcm(89151, 8325) == 247394025);
   CHECK(ranges::lcm(13361, 2842) == 37971962);
   CHECK(ranges::lcm(26447, 2813) == 74395411);
   CHECK(ranges::lcm(49158, 1870) == 45962730);
   CHECK(ranges::lcm(21934, 6433) == 141101422);

   CHECK(ranges::lcm(64207, 5685) == 365016795);
   CHECK(ranges::lcm(52766, 1871) == 98725186);
   CHECK(ranges::lcm(75560, 9911) == 748875160);
   CHECK(ranges::lcm(63373, 6343) == 401974939);
   CHECK(ranges::lcm(989, 3416) == 3378424);

   CHECK(ranges::lcm(44027, 5362) == 236072774);
   CHECK(ranges::lcm(78931, 4051) == 319749481);
   CHECK(ranges::lcm(73225, 547) == 40054075);
   CHECK(ranges::lcm(57015, 139) == 7925085);
   CHECK(ranges::lcm(91574, 7632) == 349446384);

   CHECK(ranges::lcm(47459, 6453) == 306252927);
   CHECK(ranges::lcm(89702, 9748) == 437207548);
   CHECK(ranges::lcm(59445, 2569) == 152714205);
   CHECK(ranges::lcm(67554, 593) == 40059522);
   CHECK(ranges::lcm(90308, 3032) == 68453464);

   CHECK(ranges::lcm(84212, 5648) == 118907344);
   CHECK(ranges::lcm(68613, 872) == 59830536);
   CHECK(ranges::lcm(88001, 1799) == 158313799);
   CHECK(ranges::lcm(17794, 3139) == 55855366);
   CHECK(ranges::lcm(13497, 1329) == 5979171);

   CHECK(ranges::lcm(30286, 1250) == 18928750);
   CHECK(ranges::lcm(24671, 4445) == 109662595);
   CHECK(ranges::lcm(22371, 312) == 2326584);
   CHECK(ranges::lcm(599, 9562) == 5727638);
   CHECK(ranges::lcm(64881, 8637) == 186792399);

   CHECK(ranges::lcm(31791, 9147) == 96930759);
   CHECK(ranges::lcm(19533, 186) == 1211046);
   CHECK(ranges::lcm(40307, 7024) == 283116368);
   CHECK(ranges::lcm(8347, 8959) == 4398869);
   CHECK(ranges::lcm(16151, 3605) == 58224355);

   CHECK(ranges::lcm(6925, 1632) == 11301600);
   CHECK(ranges::lcm(62446, 9113) == 569070398);
   CHECK(ranges::lcm(37655, 4396) == 165531380);
   CHECK(ranges::lcm(33797, 7128) == 240905016);
   CHECK(ranges::lcm(44148, 824) == 9094488);

   CHECK(ranges::lcm(85961, 1461) == 125589021);
   CHECK(ranges::lcm(39578, 6393) == 253022154);
   CHECK(ranges::lcm(89110, 3835) == 68347370);
   CHECK(ranges::lcm(31134, 6583) == 204955122);
   CHECK(ranges::lcm(17867, 8400) == 150082800);

   CHECK(ranges::lcm(4807, 9830) == 47252810);
   CHECK(ranges::lcm(28192, 3) == 84576);
   CHECK(ranges::lcm(16742, 8670) == 72576570);
   CHECK(ranges::lcm(61045, 6247) == 381348115);
   CHECK(ranges::lcm(647, 7286) == 4714042);

   CHECK(ranges::lcm(61770, 5095) == 62943630);
   CHECK(ranges::lcm(4615, 2695) == 2487485);
   CHECK(ranges::lcm(6525, 8163) == 5918175);
   CHECK(ranges::lcm(9199, 4120) == 37899880);
   CHECK(ranges::lcm(17800, 7442) == 66233800);

   CHECK(ranges::lcm(48763, 9344) == 455641472);
   CHECK(ranges::lcm(80768, 3206) == 129471104);
   CHECK(ranges::lcm(76210, 2935) == 44735270);
   CHECK(ranges::lcm(49881, 5649) == 93925923);
   CHECK(ranges::lcm(93863, 7599) == 713264937);

   CHECK(ranges::lcm(5302, 7733) == 3727306);
   CHECK(ranges::lcm(9772, 6516) == 15918588);
   CHECK(ranges::lcm(57760, 7635) == 88199520);
   CHECK(ranges::lcm(71250, 9839) == 701028750);
   CHECK(ranges::lcm(92847, 8323) == 772765581);

   CHECK(ranges::lcm(65937, 5944) == 391929528);
   CHECK(ranges::lcm(81995, 7994) == 655468030);
   CHECK(ranges::lcm(98618, 9530) == 469914770);
   CHECK(ranges::lcm(45248, 2628) == 29727936);
   CHECK(ranges::lcm(16428, 4855) == 79757940);

   CHECK(ranges::lcm(53911, 8978) == 484012958);
   CHECK(ranges::lcm(89153, 3762) == 335393586);
   CHECK(ranges::lcm(5210, 1467) == 7643070);
   CHECK(ranges::lcm(19236, 8331) == 53418372);
   CHECK(ranges::lcm(43667, 9076) == 396321692);

   CHECK(ranges::lcm(52746, 5723) == 5116362);
   CHECK(ranges::lcm(49100, 2201) == 108069100);
   CHECK(ranges::lcm(90249, 5082) == 152881806);
   CHECK(ranges::lcm(68254, 5070) == 173023890);
   CHECK(ranges::lcm(67767, 8889) == 200793621);

   CHECK(ranges::lcm(34462, 765) == 26363430);
   CHECK(ranges::lcm(55565, 4715) == 52397795);
   CHECK(ranges::lcm(50778, 5188) == 131718132);
   CHECK(ranges::lcm(43850, 9088) == 199254400);
   CHECK(ranges::lcm(93114, 3921) == 121699998);

   return ::test_result();
}
