// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2016
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
#include <stl2/detail/numeric/inner_product.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"
#include <deque>
#include <list>
#include <numeric>
#include <stl2/detail/concepts/number.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include <iostream>

namespace ranges = __stl2;

template <typename U, typename V>
void CHECK_algorithm(const U& u, const V& v)
{
   auto s = std::inner_product(u.begin(), u.end(), v.begin(), ranges::value_type_t<U>{});

   CHECK(s == ranges::inner_product(u.begin(), u.end(), v.begin(), ranges::value_type_t<U>{}));
   CHECK(s == ranges::inner_product(u, v.begin()));
}

template <typename U, typename V, typename Proj1, typename Proj2>
void CHECK_projection(U u, V v, Proj1 proj1, Proj2 proj2)
{
   using Value_type = ranges::value_type_t<U>;
   auto s = std::inner_product(u.begin(), u.end(), v.begin(), Value_type{},
                           [](const auto& i, const auto& j) {
                              return std::plus<Value_type>{}(i, j);
                           },
                           [](const auto& i, const auto& j) {
                              return std::multiplies<Value_type>{}(i, j.second);
                           });
   auto r = ranges::inner_product(u.begin(), u.end(), v.begin(), Value_type{},
                                  ranges::plus<Value_type>{},
                                  ranges::multiplies<Value_type>{},
                                  ranges::move(proj1),
                                  ranges::move(proj2));
   CHECK(s == r);
   CHECK(r == ranges::inner_product(u, v.begin(), Value_type{},
                                    ranges::plus<Value_type>{},
                                    ranges::multiplies<Value_type>{},
                                    ranges::move(proj1),
                                    ranges::move(proj2)));
}

int main()
{
   CHECK_algorithm(std::vector<int>{}, std::vector<int>{});
   CHECK_algorithm(std::deque<int>{}, std::vector<double>{});
   CHECK_algorithm(std::vector<double>{}, std::list<int>{});

   CHECK_algorithm(std::vector<int>{}, std::vector<int>{1});
   CHECK_algorithm(std::vector<int>{}, std::vector<double>{1});
   CHECK_algorithm(std::vector<double>{}, std::vector<int>{1});

   // CHECK_algorithm(std::vector<int>{1, 2, 3}, std::vector<int>{});
   CHECK_algorithm(std::vector<int>{1}, std::vector<int>{2});
   CHECK_algorithm(std::vector<int>{10}, std::deque<float>{3.0f});
   CHECK_algorithm(std::list<double>{4.0}, std::list<int>{7});

   CHECK_algorithm(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                   std::vector<int>{10, 20, 30, 40, 50, 60, 70, 80, 90});
   CHECK_algorithm(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                   std::deque<double>{1.0, 2.1, 3.2, 4.3, 5.4, 6.5, 7.6, 8.7, 9.8, 10.9});
   CHECK_algorithm(std::vector<double>{-7.0, 5.3, 3.1415965},
                   std::list<int>{0xa, 0xb, 0xc, 0xd});

   CHECK_projection(std::vector<int>{0, 1, 2, 3, 4, 5},
                    std::unordered_map<std::string, int>{{"hello", 15},
                                                         {"world", 20},
                                                         {"first", 25},
                                                         {"last", 30},
                                                         {"begin", 35},
                                                         {"end", 40}},
                    ranges::identity{},
                    [](const auto& i){ return i.second; });
   return test_result();
}